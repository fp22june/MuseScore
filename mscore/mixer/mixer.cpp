//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//
//  Copyright (C) 2002-2016 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "mixer.h"

#include <QtGlobal>
#include <qmessagebox.h>
#include <accessibletoolbutton.h>
#include "musescore.h"
#include "libmscore/excerpt.h"
#include "libmscore/score.h"
#include "libmscore/part.h"
#include "libmscore/instrument.h"
#include "seq.h"
#include "libmscore/undo.h"
#include "synthcontrol.h"
#include "audio/midi/msynthesizer.h"
#include "preferences.h"
#include "enableplayforwidget.h"

#include "mixerdetails.h"
#include "mixertreewidget.h"
#include "mixertreerowwidget.h"
#include "mixermasterchannel.h"
#include "mixertreerow.h"
#include "mixeroptions.h"
#include "mixeroptionsbutton.h"

namespace Ms {
MixerOptions* Mixer::options = new MixerOptions();
Mixer::Mixer(QWidget* parent)
      : QDockWidget("Mixer", parent)
      {
      enablePlay = new EnablePlayForWidget(this); // showEvent before setupUi // TODO
      setupUi(this);
      _score = nullptr;
      _activeScore = nullptr;
      masterChannelWidget = new MixerMasterChannel();
      mixerDetails = new MixerDetails(this);  // mixer->tree after setupUi 
      setWindowFlags(Qt::Tool);
      setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
      mixerTreeWidget->setMixer(this);
      showDetailsButton->setTarget(this);

      masterVolumeTreeWidget->clear();
      masterVolumeTreeWidget->setColumnCount(2);
      masterVolumeTreeWidget->header()->setSectionResizeMode(0, QHeaderView::Fixed);
      masterVolumeTreeWidget->header()->setSectionResizeMode(1, QHeaderView::Fixed);
      masterVolumeTreeWidget->setSelectionMode(QAbstractItemView::NoSelection);
      QTreeWidgetItem* masterVolumeItem = new QTreeWidgetItem(masterVolumeTreeWidget);
      masterVolumeItem->setText(0, tr("Master"));
      masterVolumeTreeWidget->addTopLevelItem(masterVolumeItem);
      masterVolumeTreeWidget->setItemWidget(masterVolumeItem, 1, masterChannelWidget);
      connect(mixerTreeWidget, &MixerTreeWidget::headerWidthChanged, this, &Mixer::treeHeaderWidthChangeListener);

      MixerViewParam m{};
      m.optionchange = true;
      view(m);

      keyboardFilter = new MixerKeyboardControlFilter(this);
      installEventFilter(keyboardFilter);
      mixerTreeWidget->installEventFilter(keyboardFilter);

      connect(partOnlyCheckBox, SIGNAL(toggled(bool)), SLOT(partOnlyCheckBoxToggled(bool)));

      retranslate(true);
      }
// back (including Tree and TreeRow model)
void Mixer::controlUiEventListener(QString origin, MixerTreeRow* item, QString control, controlEventParam value) {
      if (origin == "MixerTreeRowWidget") {
            if (item->itemType() == MixerTreeRow::ItemType::INSTRUMENT) {
                  if (control == "stripSliderMoved") {
                        }
                  //else if (control == "stripSoloToggled") {
                  //      }
                  //else if (control == "stripMuteToggled") {
                  //      }
                  //else if (control == "trackColorEdited") {
                  //      }
                  }
            else if (item->itemType() == MixerTreeRow::ItemType::CHANNEL) {
                  if (control == "stripSliderMoved") {
                        // data ; triggers channelPropChangeEventListener() in turn Mixer::view()
                        qInfo() << item->text(0) ;
                        MixerTreeRow* p = item;
                        while (p = p->parent()) {
                              qInfo() << p->text(0);
                              }
                        if (!Mixer::getOptions()->secondaryMode()) 
                              setChannelVolume(item->channel(), value.i);
                        else {
                              switch (Mixer::getOptions()->secondarySlider()) {
                                    case MixerOptions::MixerSecondarySlider::Pan:
                                          setChannelPan(item->channel(), value.i + PANSLIDERSHIFT);
                                          break;
                                    case MixerOptions::MixerSecondarySlider::Reverb:
                                          setChannelReverb(item->channel(), value.i);
                                          break;
                                    case MixerOptions::MixerSecondarySlider::Chorus:
                                          setChannelChorus(item->channel(), value.i);
                                          break;
                                    }
                              }
                        }
                  //else if (control == "stripSoloToggled") {
                  //      }
                  //else if (control == "stripMuteToggled") {
                  //      }
                  //else if (control == "trackColorEdited") {
                  //      }
                  }
            }
      }
void Mixer::channelPropChangeEventListener(Channel* channel, Channel::Prop property) { updateChannelView(channel, property); }
void Mixer::updateChannelViewAutoProp(Channel* channel) {
      if (!Mixer::getOptions()->secondaryMode()) updateChannelView(channel, Channel::Prop::VOLUME);
      else if (Mixer::getOptions()->secondarySlider() == MixerOptions::MixerSecondarySlider::Pan   ) updateChannelView(channel, Channel::Prop::PAN);
      else if (Mixer::getOptions()->secondarySlider() == MixerOptions::MixerSecondarySlider::Reverb) updateChannelView(channel, Channel::Prop::REVERB);
      else if (Mixer::getOptions()->secondarySlider() == MixerOptions::MixerSecondarySlider::Chorus) updateChannelView(channel, Channel::Prop::CHORUS);
      }
void Mixer::updateChannelView(Channel* channel, Channel::Prop property) {
      MixerViewParam m{}, m2{};
      if ( property == Channel::Prop::VOLUME || property == Channel::Prop::PAN || property == Channel::Prop::REVERB || property == Channel::Prop::CHORUS) {
            if (!Mixer::getOptions()->secondaryMode()) {
                  m.channel = channel;
                  m.item = "volume";
                  m.i = getChannelVolume(channel);
                  view(m);

                  m2.channel = channel;
                  m2.item = "tooltip";
                  m2.s = tr("Volume: %1").arg(QString::number(getChannelVolume(channel)));
                  view(m2);
                  }
            else {
                  switch (Mixer::getOptions()->secondarySlider()) {
                        case MixerOptions::MixerSecondarySlider::Pan:
                              m.channel = channel;
                              m.item = "pan";
                              m.i = getChannelPan(channel) - PANSLIDERSHIFT;
                              view(m);

                              m2.channel = channel;
                              m2.item = "tooltip";
                              m2.s = tr("Pan: %1").arg(QString::number(getChannelPan(channel)));
                              view(m2);
                              break;
                        case MixerOptions::MixerSecondarySlider::Reverb:
                              m.channel = channel;
                              m.item = "reverb";
                              m.i = getChannelReverb(channel);
                              view(m);

                              m2.channel = channel;
                              m2.item = "tooltip";
                              m2.s = tr("Reverb: %1").arg(QString::number(getChannelReverb(channel)));
                              view(m2);
                              break;
                        case MixerOptions::MixerSecondarySlider::Chorus:
                              m.channel = channel;
                              m.item = "chorus";
                              m.i = getChannelChorus(channel);
                              view(m);

                              m2.channel = channel;
                              m2.item = "tooltip";
                              m2.s = tr("Chorus: %1").arg(QString::number(getChannelChorus(channel)));
                              view(m2);
                              break;
                        }
                  }
            }
      }
void Mixer::soundFontChangeListener() {
      MixerViewParam m{};
      m.refreshTree = true;
      view(m);
      }
void Mixer::playbackScoreChangeListener() {
      MixerViewParam m{};
      m.refreshTree = true;
      view(m);
      }
void Mixer::midiPrefsChanged(bool) { // entry Preferences // TODO
      /*emit*/ playbackScoreChangeListener();
      }
void Mixer::setScore(Score* score) { // entry musescore.cpp and MuseScore::showMixer
      _activeScore = score;
      _score = _activeScore ? _activeScore->masterScore()->playbackScore() : nullptr;
      /*emit*/ playbackScoreChangeListener();
      partOnlyCheckBox->setChecked(mscore->playPartOnly()); //TODO
      partOnlyCheckBox->setEnabled(_activeScore && !_activeScore->isMaster()); //TODO
      }
void Mixer::treeHeaderWidthChangeListener(QHeaderView* header) {
      masterVolumeTreeWidget->header()->resizeSection(0, header->sectionSize(0)); //TODO
      masterVolumeTreeWidget->header()->resizeSection(1, header->sectionSize(1)); //TODO
      }
void Mixer::setSliderSecMode(bool x) {
      options->setsecondaryMode(x);
      mixerTreeWidget->allRowSendSliderUpdateRequest();
      }
void Mixer::optionChangeListener() {
      MixerViewParam m{};
      m.optionchange = true;
      view(m);
      }
void Mixer::partOnlyCheckBoxToggled(bool checked) {
      if (!_activeScore || !_activeScore->excerpt())
            return;
      mscore->setPlayPartOnly(checked);
      /*emit*/ playbackScoreChangeListener();
      for (const MidiMapping& mm : _activeScore->masterScore()->midiMapping()) {
            const Channel* ch = mm.articulation();
            if (ch && (ch->mute() || ch->soloMute()))
                  seq->stopNotes(ch->channel());
            }
      }
// front
void MuseScore::showMixer(bool visible) {
      QAction* toggleMixerAction = getAction("toggle-mixer");
      if (mixer == 0) {
            mixer = new Mixer(this);
            mscore->stackUnder(mixer);
            if (synthControl)
                  connect(synthControl, SIGNAL(updateMixer()), mixer, SLOT(soundFontChangeListener()));
            connect(synti, SIGNAL(soundFontChanged()), mixer, SLOT(soundFontChangeListener())); //msynthesizer.cpp
            connect(mixer, SIGNAL(closed(bool)), toggleMixerAction, SLOT(setChecked(bool)));
            mixer->setFloating(false);
            addDockWidget(Qt::RightDockWidgetArea, mixer);
            }
      reDisplayDockWidget(mixer, visible);
      toggleMixerAction->setChecked(visible);
      mixer->setScore(cs);
      }
void Mixer::view(MixerViewParam& m) {
      // tree
      if (m.optionchange) {
            //details
            showDetails(options->showingDetails());
            if (options->showDetailsOnTheSide()) {
                  h1->addWidget(mixerDetails, 0, Qt::AlignTop);
                  }
            else {
                  v1->addWidget(mixerDetails, 0, Qt::AlignTop);
                  }
            mixerDetails->updateUiOptions();
            //master
            masterChannelWidget->updateUiControls();
            masterVolumeTreeWidget->setVisible(options->showMasterVolume());
            //tree
            if(mixerTreeWidget) mixerTreeWidget->allRowSendSliderUpdateRequest();
            MTViewPayload p{};
            p.updateHeaders = true;
            mixerTreeWidget->view(p);
            }
      if (m.refreshTree) {
            MTViewPayload p{};
            p.refreshTree = true;
            p.score = _score;
            mixerTreeWidget->view(p);
            }
      if (m.channel) {
            if (m.item == "volume" || m.item == "pan" || m.item == "reverb" || m.item == "chorus") {
                  MTViewPayload p{};
                  p.all = true;
                  MTRViewPayload r{};
                  r.updateSecMode = true;
                  p.rowPayload = &r;
                  mixerTreeWidget->view(p);
            }
            if (     (     m.item == "volume"
                        && !Mixer::getOptions()->secondaryMode()
                        )
                  || (     m.item == "pan"
                        && Mixer::getOptions()->secondaryMode()
                        && Mixer::getOptions()->secondarySlider() == MixerOptions::MixerSecondarySlider::Pan
                        )
                  || (     m.item == "reverb"
                        && Mixer::getOptions()->secondaryMode()
                        && Mixer::getOptions()->secondarySlider() == MixerOptions::MixerSecondarySlider::Reverb
                        )
                  || (     m.item == "chorus"
                        && Mixer::getOptions()->secondaryMode()
                        && Mixer::getOptions()->secondarySlider() == MixerOptions::MixerSecondarySlider::Chorus
                        )
                  ) {
                  MTViewPayload p{};
                  p.channel = m.channel;
                  MTRViewPayload r{};
                  r.setSlider = true;
                  r.slider = m.i;
                  p.rowPayload = &r;
                  mixerTreeWidget->view(p);
                  }
            if (m.item == "tooltip") {
                  MTViewPayload p{};
                  p.channel = m.channel;
                  MTRViewPayload r{};
                  r.setToolTip = true;
                  r.toolTip = m.s;
                  p.rowPayload = &r;
                  mixerTreeWidget->view(p);
                  }
            }
      }
void Mixer::showDetails(bool visible) {
      QSize currentTreeWidgetSize = mixerTreeWidget->size();
      QSize minTreeWidgetSize = mixerTreeWidget->minimumSize();
      QSize maxTreeWidgetSize = mixerTreeWidget->maximumSize();
      if (!isFloating() && visible) {
            // Special case - make the mixerTreeView as narrow as possible before showing the
            // detailsView. Without this step, mixerTreeView will be as fully wide as the dock
            // and when the detailsView is added it will get even wider. (And if the user toggles QT
            // will keep making the dock / mainWindow wider and wider, which is highly undesirable.)
            mixerTreeWidget->setMaximumSize(minTreeWidgetSize);
            mixerDetails->setVisible(visible);
            dockWidgetContents->adjustSize();
            mixerTreeWidget->setMaximumSize(maxTreeWidgetSize);
            return;
            }
      // Pin the size of the mixerView when either showing or hiding the details view.
      // This ensures that the mixer window (when undocked) will shrink or grow as
      // appropriate.
      mixerTreeWidget->setMinimumSize(currentTreeWidgetSize);
      mixerTreeWidget->setMaximumSize(currentTreeWidgetSize);
      mixerDetails->setVisible(visible);
      mixerTreeWidget->adjustSize();
      dockWidgetContents->adjustSize();
      this->adjustSize(); // All three adjustSize() calls (appear) to be required
      mixerTreeWidget->setMinimumSize(minTreeWidgetSize);
      mixerTreeWidget->setMaximumSize(maxTreeWidgetSize);
      }
void Mixer::retranslate(bool firstTime) {
      setWindowTitle(tr("Mixer"));
      if (firstTime)
            return;
      retranslateUi(this);
      mixerDetails->retranslateUi(mixerDetails);
      MTViewPayload p{};
      p.updateHeaders = true;
      mixerTreeWidget->view(p);
      }
void Mixer::changeEvent(QEvent *event) {
      QDockWidget::changeEvent(event);
      if (event->type() == QEvent::LanguageChange)
            retranslate();
      }
// getter
QString Mixer::getPartName(Part* part) { return part->partName(); }
QString Mixer::getInstrumentName(Instrument* instrument) { return instrument->trackName(); }
QString Mixer::getChannelName(Channel* channel) {
      if (channel->name().isEmpty())
            return "";
      return qApp->translate("InstrumentsXML", channel->name().toUtf8().data());
      }
int Mixer::getChannelVolume(Channel* channel) { return (int)(channel->volume()); }
int Mixer::getChannelChorus(Channel* channel) { return (int)(channel->chorus()); }
int Mixer::getChannelReverb(Channel* channel) { return (int)(channel->reverb()); }
int Mixer::getChannelPan(Channel* channel) { return (int)(channel->pan()); }
bool Mixer::getChannelMute(Channel* channel) { return channel->mute(); }
bool Mixer::getChannelSolo(Channel* channel) { return channel->solo(); }
int Mixer::getChannelMidiChannel(Channel* channel) { return _activeScore->masterScore()->midiMapping(channel->channel())->channel() + 1; }
int Mixer::getChannelMidiPort(Channel* channel) { return _activeScore->masterScore()->midiMapping(channel->channel())->port() + 1; }
QList<QList<bool>> Mixer::getPartMutedVoices(Part* part) {
      QList<QList<bool>> mutedStaves;
      for (int staffIndex = 0; staffIndex < (*part->staves()).length(); ++staffIndex) {
            Staff* staff = (*part->staves())[staffIndex];
            QList<bool> mutedVoices;
            for (int voice = 0; voice < VOICES; ++voice) {
                  bool checked = !staff->playbackVoice(voice);
                  mutedVoices.append(checked);
                  }
            mutedStaves.append(mutedVoices);
            }
      return mutedStaves;
      }
int Mixer::getPartColor(Part* part) { return part->color(); }
int Mixer::getIntrumentColor(Instrument* instrument) { return instrument->color(); }
int Mixer::getChannelColor(Channel* channel) { return channel->color(); }
MidiMapping* Mixer::getMidiMap(Channel* channel) { return _activeScore->masterScore()->midiMapping(channel->channel()); }
Channel* Mixer::getPlaybackChannel(Channel* channel) { return _activeScore->masterScore()->playbackChannel(channel); }
bool Mixer::getUseDrumset(Channel* channel) { return _activeScore->masterScore()->midiMapping(channel->channel())->part()->instrument()->useDrumset(); } //Check if drumkit

// validator setter //TODO undo.cpp
void Mixer::setPartName(Part* part, QString newName) {
      if (part->partName() == newName)
            return;
      Score* score = part->score();
      if (score) {
            score->startCmd();
            score->undo(new ChangePart(part, part->instrument()/*default instrument*/, newName));
            score->endCmd();
            }
      }
void Mixer::setChannelName(Channel* channel, QString newName) {
      channel->setName(newName);
      }
void Mixer::setChannelVolume(Channel* channel, int value) {
      channel->setVolume(std::max(0, std::min(127, value)));
      seq->setController(channel->channel(), CTRL_VOLUME, channel->volume()); 
      }
void Mixer::setChannelMute(Channel* channel, bool muteOn) {
      if(muteOn) seq->stopNotes(channel->channel());
      channel->setMute(muteOn);
      }
void Mixer::setChannelSolo(Channel* channel, bool soloOn) {
      if (soloOn) seq->stopNotes(channel->channel());
      channel->setSolo(soloOn);
      }
void Mixer::setChannelPan(Channel* channel, int value) {
      channel->setPan(std::max(0, std::min(127, value)));
      seq->setController(channel->channel(), CTRL_PANPOT, channel->pan());
      }
void Mixer::setChannelChorus(Channel* channel, int value) {
      channel->setChorus(std::max(0, std::min(127, value)));
      seq->setController(channel->channel(), CTRL_CHORUS_SEND, channel->chorus()); 
      }
void Mixer::setChannelReverb(Channel* channel, int value) {
      channel->setReverb(std::max(0, std::min(127, value)));
      seq->setController(channel->channel(), CTRL_REVERB_SEND, channel->reverb()); 
      }
void Mixer::togglePartMutedVoice(Part* part, int staffIndex, int voiceIndex, bool shouldMute) {
      Staff* staff = part->staff(staffIndex);
      switch (voiceIndex) {
            case 0:
                  staff->undoChangeProperty(Pid::PLAYBACK_VOICE1, !shouldMute);
                  break;
            case 1:
                  staff->undoChangeProperty(Pid::PLAYBACK_VOICE2, !shouldMute);
                  break;
            case 2:
                  staff->undoChangeProperty(Pid::PLAYBACK_VOICE3, !shouldMute);
                  break;
            case 3:
                  staff->undoChangeProperty(Pid::PLAYBACK_VOICE4, !shouldMute);
                  break;
            }
      }
void Mixer::setPartColor(Part* part, int valueRgb) {
      part->setColor(valueRgb);
      }
void Mixer::setIntrumentColor(Part* part, Instrument* instrument, int valueRgb) {
      // const InstrumentList* instrumentList = part->instruments();
      // for (auto mapIterator = instrumentList->begin(); mapIterator != instrumentList->end(); ++mapIterator) {
      //       Instrument* i = mapIterator->second;
      //       if (instrument == i)
      //             i->setColor(valueRgb);
      //       // for (const Channel* instrumentChannel: i->channel()) {
      //       //       playbackChannel(instrumentChannel)->setColor(valueRgb);
      //       //       }
      //       }
      instrument->setColor(valueRgb);
      }
void Mixer::setChannelColor(Channel* channel, int valueRgb) { channel->setColor(valueRgb); }

void Mixer::setMidiChannelAndPort(Part* part, Channel* channel, int midiChannel, int midiPort) {
      seq->stopNotes(channel->channel());
      midiPort = midiPort - 1;
      midiChannel = midiChannel - 1;

      _activeScore->masterScore()->updateMidiMapping(getMidiMap(channel)->articulation(), part, midiPort, midiChannel);

      _activeScore->setInstrumentsChanged(true);
      _activeScore->setLayoutAll();
      seq->initInstruments();

      // Update MIDI Out ports
      int maxPort = std::max(midiPort, _activeScore->masterScore()->midiPortCount());
      _activeScore->masterScore()->setMidiPortCount(maxPort);
      if (seq->driver() && (preferences.getBool(PREF_IO_JACK_USEJACKMIDI) || preferences.getBool(PREF_IO_ALSA_USEALSAAUDIO)))
            seq->driver()->updateOutPortCount(maxPort + 1);
      }

// window events
void Mixer::closeEvent(QCloseEvent* ev) {
      emit closed(false);
      QDockWidget::closeEvent(ev);
      }
void Mixer::showEvent(QShowEvent* e) {
      if (e->spontaneous() && !isFloating()) {
            QDockWidget::showEvent(e);
            }
      else {
            enablePlay->showEvent(e);
            QDockWidget::showEvent(e);
            activateWindow();
            setFocus();
            }
      if (!e->spontaneous())
            getAction("toggle-mixer")->setChecked(true);
      }
void Mixer::hideEvent(QHideEvent* e) {
      QDockWidget::hideEvent(e);
      if (!e->spontaneous())
            getAction("toggle-mixer")->setChecked(false);
      }
// keyboard
bool Mixer::eventFilter(QObject* object, QEvent* event) {
      if (enablePlay->eventFilter(object, event))
            return true;
      return QWidget::eventFilter(object, event);
      }
void Mixer::keyPressEvent(QKeyEvent* ev) {
      // ctrl
      bool focus = hasFocus();
      if (!focus) {
            QWidget* focusWidget = QApplication::focusWidget();
            focus =  focusWidget && isAncestorOf(focusWidget)
                  && !(    dynamic_cast<QLineEdit*>(focusWidget)
                        || dynamic_cast<QSpinBox*>(focusWidget)
                        || dynamic_cast<QDoubleSpinBox*>(focusWidget));
            }
      if (!focus)
            return;
      if (QApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier) {
            setSliderSecMode(true);
            return;
            }
      // close
      // if (ev->key() == Qt::Key_Escape && ev->modifiers() == Qt::NoModifier) {
      //       close();
      //       return;
      //       }
      // super
      QDockWidget::keyPressEvent(ev);
      }
void Mixer::keyReleaseEvent(QKeyEvent* ev) {
      // ctrl
      if (!(QApplication::queryKeyboardModifiers() & Qt::KeyboardModifier::ControlModifier))
            setSliderSecMode(false);
      // super
      QDockWidget::keyReleaseEvent(ev);
      }

// TODO
int Mixer::nudge(int currentValue, NudgeDirection direction, int lowerLimit, int upperLimit) {
      int proposedValue = currentValue + (direction == NudgeDirection::Up ? 1 : -1);
      if (currentValue > upperLimit)
            proposedValue = upperLimit;
      if (currentValue < lowerLimit)
            proposedValue = lowerLimit;
      return proposedValue;
      }
void Mixer::nudgeMainSlider(NudgeDirection direction) {
      MixerTreeRow* treeRow = mixerDetails->getSelectedMixerTreeRow();
      int proposedValue = nudge(getChannelVolume(treeRow->channel()), direction, 0, 127);
      setChannelVolume(treeRow->channel(), proposedValue);
      }
void Mixer::nudgeSecondarySlider(NudgeDirection direction) {
      MixerTreeRow* treeRow = mixerDetails->getSelectedMixerTreeRow();
      int proposedValue;
      switch (options->secondarySlider()) {
            case MixerOptions::MixerSecondarySlider::Pan:
                  proposedValue = nudge(getChannelPan(treeRow->channel()), direction, -64, 63);
                  setChannelPan(treeRow->channel(), proposedValue);
                  break;
            case MixerOptions::MixerSecondarySlider::Reverb:
                  proposedValue = nudge(getChannelReverb(treeRow->channel()), direction, 0, 127);
                  setChannelReverb(treeRow->channel(), proposedValue);
                  break;
            case MixerOptions::MixerSecondarySlider::Chorus:
                  proposedValue = nudge(getChannelChorus(treeRow->channel()), direction, 0, 127);
                  setChannelChorus(treeRow->channel(), proposedValue);
                  break;
            }
      }

MixerKeyboardControlFilter::MixerKeyboardControlFilter(Mixer* mixer) {
      _mixer = mixer;
      }
bool MixerKeyboardControlFilter::eventFilter(QObject *obj, QEvent *event) {
      if (event->type() != QEvent::KeyPress) {
            return QObject::eventFilter(obj, event);
      }

      MixerTreeRow* selectedMixerTreeRow = _mixer->mixerDetails->getSelectedMixerTreeRow();

      if (!selectedMixerTreeRow)
            return QObject::eventFilter(obj, event);

      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

      bool modified = keyEvent->modifiers() == Qt::ControlModifier;
      bool secondaryLock = Mixer::getOptions()->sliderSecModeInvert();
      modified = secondaryLock ? !modified : modified;

      Qt::Key primaryDown = !secondaryLock ? Qt::Key_Comma : Qt::Key_Less;
      Qt::Key primaryUp = !secondaryLock ? Qt::Key_Period : Qt::Key_Greater;
      Qt::Key secondaryDown = !secondaryLock ? Qt::Key_Less : Qt::Key_Comma;
      Qt::Key secondaryUp = !secondaryLock ? Qt::Key_Greater : Qt::Key_Period;

      if (keyEvent->key() == primaryDown && !modified) {
            _mixer->nudgeMainSlider(Mixer::NudgeDirection::Down);
            return true;
            }
      if (keyEvent->key() == primaryUp && !modified) {
            _mixer->nudgeMainSlider(Mixer::NudgeDirection::Up);
            return true;
            }
      if (keyEvent->key() == secondaryDown && modified) {
            _mixer->nudgeSecondarySlider(Mixer::NudgeDirection::Down);
            return true;
            }
      if (keyEvent->key() == secondaryUp && modified) {
            _mixer->nudgeSecondarySlider(Mixer::NudgeDirection::Up);
            return true;
            }
      //if (keyEvent->key() == Qt::Key_M && keyEvent->modifi/*ers() == Qt::NoModifier) {
      //      if (selectedMixerTreeRow) {
      //            selectedMixerTreeRow->setMute(!selectedMixerTreeRow->getMute());
      //            }
      //      return true;
      //      }
      //if (keyEvent->key() == Qt::Key_S && keyEvent->modifiers() == Qt::NoModifier) {
      //      if (selectedMixerTreeRow) {
      //            selectedMixerTreeRow->setSolo(!selectedMixerTreeRow->getSolo());
      //            }
      //      return true;
      //      }*/
      return QObject::eventFilter(obj, event);
      }
}
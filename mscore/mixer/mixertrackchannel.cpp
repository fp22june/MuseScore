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

#include "mixertrackchannel.h"

#include "libmscore/instrument.h"
#include "musescore.h"
#include "audio/midi/msynthesizer.h"     // required for MidiPatch

#include "mixer.h"
#include "mixertrackitem.h"
#include "mixeroptions.h"
#include "awl/colorlabel.h"
// =======
// #include "seq.h"
// #include "libmscore/undo.h"
// #include "synthcontrol.h"
// #include "audio/midi/msynthesizer.h"
// #include "preferences.h"
// >>>>>>> 3.x:mscore/mixer/mixertrackchannel.cpp

namespace Ms {

//--------------------------------------------------------------
//  MixerTrackChannel provides an widget that is displayed in a
//  row of a QTreeWidget. The widget includes a slider (by default
//  to control track volume) and Mute and Solo Buttons. The widget
//  is a "listener" to the track that is controls. This means that
//  when other parts of MuseScore change the track, the control
//  will update itself.
//--------------------------------------------------------------
MixerTrackChannel::MixerTrackChannel(MixerTreeWidgetItem* treeWidgetItem) :
      treeWidgetItem(treeWidgetItem)
      {
      setupUi(this);
      setupAdditionalUi();
      updateUiControls();
      setupSlotsAndSignals();
      update();

      Channel* channel = mixerTrackItem()->channel();
      channel->addListener(this);
// =======
//       connect(soloBn, SIGNAL(toggled(bool)), SLOT(updateSolo(bool)));
//       connect(muteBn, SIGNAL(toggled(bool)), SLOT(updateMute(bool)));

//       updateNameLabel();

//       //set up rest
//       Channel* chan = mti->chan();
//       soloBn->setChecked(chan->solo());
//       muteBn->setChecked(chan->mute());

//       chan->addListener(this);
//       volumeSlider->setValue(chan->volume());
//       volumeSlider->setToolTip(tr("Volume: %1").arg(QString::number(chan->volume())));
//       volumeSlider->setMaxValue(127);
//       volumeSlider->setMinValue(0);
//       volumeSlider->setDoubleClickValue(Channel::defaultVolume);
//       volumeSlider->setNumMajorTicks(10);
//       volumeSlider->setNumMinorTicks(5);

//       QIcon iconSliderHead;
//       iconSliderHead.addFile(QStringLiteral(":/data/icons/mixer-slider-handle-vertical.svg"), QSize(), QIcon::Normal, QIcon::Off);
//       volumeSlider->setSliderHeadIcon(iconSliderHead);

//       panSlider->setValue(chan->pan());
//       panSlider->setToolTip(tr("Pan: %1").arg(QString::number(chan->pan())));
//       panSlider->setMaxValue(127);
//       panSlider->setMinValue(0);

//       connect(volumeSlider, SIGNAL(valueChanged(double)),      SLOT(volumeChanged(double)));
//       connect(panSlider,    SIGNAL(valueChanged(double, int)), SLOT(panChanged(double)));

//       connect(volumeSlider, SIGNAL(sliderPressed()),    SLOT(controlSelected()));
//       connect(panSlider,    SIGNAL(sliderPressed(int)), SLOT(controlSelected()));

//       applyStyle();
// >>>>>>> 3.x:mscore/mixer/mixertrackchannel.cpp
      }



void MixerTrackChannel::setupSlotsAndSignals()
      {
      connect(muteButton,     SIGNAL(toggled(bool)),        SLOT(stripMuteToggled(bool)));
      connect(soloButton,     SIGNAL(toggled(bool)),        SLOT(stripSoloToggled(bool)));
      connect(trackSlider,    SIGNAL(valueChanged(int)),    SLOT(stripVolumeSliderMoved(int)));
      connect(trackSlider,    SIGNAL(sliderPressed()),      SLOT(takeSelection()));
      connect(colorChooser,   SIGNAL(colorChanged(QColor)), SLOT(trackColorEdited(QColor)));

      }

void MixerTrackChannel::takeSelection()
      {
      treeWidgetItem->treeWidget()->setCurrentItem(treeWidgetItem);
      }

void MixerTrackChannel::setupAdditionalUi()
      {
      //TODO: a more responsible approach to styling that's also light/dark theme respectful
      QString basicButton = "QToolButton{background: white; color: black; font-weight: bold; border: 1px solid gray;}";
      QString colorTemplate = "QToolButton:checked, QToolButton:pressed { color: white; background: %1;}";
      muteButton->setStyleSheet(basicButton + colorTemplate.arg("red"));
      soloButton->setStyleSheet(basicButton + colorTemplate.arg("green"));
      }

void MixerTrackChannel::updateUiControls()
      {
      bool showTrackColors = Mixer::getOptions()->showTrackColors();
      colorChooser->setVisible(showTrackColors);
      bool secondaryMode = Mixer::getOptions()->secondaryModeOn();
      trackSlider->setSecondaryMode(secondaryMode);
      trackSlider->blockSignals(true);
      trackSlider->setPanMode(secondaryMode && Mixer::getOptions()->secondarySlider() == MixerOptions::MixerSecondarySlider::Pan);
      update();
      trackSlider->blockSignals(false);
      }

void MixerTrackChannel::update()
      {
      const QSignalBlocker blockVolumeSignals(trackSlider);
      const QSignalBlocker blockMuteSignals(muteButton);
      const QSignalBlocker blockSoloSignals(soloButton);

      MixerOptions* options = Mixer::getOptions();

      int value;
      QString tooltip;

      if (options->secondaryModeOn()) {
            switch (options->secondarySlider()) {
                  case MixerOptions::MixerSecondarySlider::Pan:
                        value = mixerTrackItem()->getPan();
                        tooltip = tr("Pan: %1");
                        break;
                  case MixerOptions::MixerSecondarySlider::Reverb:
                        value = mixerTrackItem()->getReverb();
                        tooltip = tr("Reverb: %1");
                        break;
                  case MixerOptions::MixerSecondarySlider::Chorus:
                        value = mixerTrackItem()->getChorus();
                        tooltip = tr("Chorus: %1");
                        break;
                  }
            }
      else {
            value = mixerTrackItem()->getVolume();
            tooltip = tr("Volume: %1");
// =======
//       QString style;
//       switch (preferences.effectiveGlobalStyle()){
//             case MuseScoreEffectiveStyleType::DARK_FUSION:
//                   style = _selected ? selStyleDark : unselStyleDark;
//                   break;
//             case MuseScoreEffectiveStyleType::LIGHT_FUSION:
//                   style = _selected ? selStyleLight : unselStyleLight;
//                   break;
// >>>>>>> 3.x:mscore/mixer/mixertrackchannel.cpp
            }


      trackSlider->setValue(value);
      trackSlider->setToolTip(tooltip.arg(QString::number(value)));
      
      muteButton->setChecked(mixerTrackItem()->getMute());
      soloButton->setChecked(mixerTrackItem()->getSolo());

      int channelColor = mixerTrackItem()->color();
      QColor tweakedColor = QColor(channelColor | 0xff000000);
      colorChooser->setColor(tweakedColor);
      //TODO: is it necesary to set the borders every time? and their color
      colorChooser->setStyleSheet(QString("QFrame{background: %1;padding-top: 2px; padding-bottom: 2px; border-radius: 3px;}").arg(tweakedColor.name()));

      setToolTip(mixerTrackItem()->detailedToolTip());
      }

// =======
// //---------------------------------------------------------
// //   updateNameLabel
// //---------------------------------------------------------

// void MixerTrackChannel::updateNameLabel()
//       {
//       Part* part = _mti->part();
//       Instrument* instr = _mti->instrument();
//       Channel* chan = _mti->chan();

//       QString shortName;
//       if (instr->shortNames().count())
//             shortName = instr->shortNames().first().name() + "-";
//       else
//             shortName = "";
//       QString text = QString("%1%2").arg(shortName, qApp->translate("InstrumentsXML", chan->name().toUtf8().data()));
//       trackLabel->setText(text);

//       MidiPatch* mp = synti->getPatchInfo(chan->synti(), chan->bank(), chan->program());

//       QString tooltip = tr("Part Name: %1\n"
//                            "Instrument: %2\n"
//                            "Channel: %3\n"
//                            "Bank: %4\n"
//                            "Program: %5\n"
//                            "Sound: %6")
//                   .arg(part->partName(),
//                        instr->trackName(),
//                        qApp->translate("InstrumentsXML", chan->name().toUtf8().data()),
//                        QString::number(chan->bank()),
//                        QString::number(chan->program()),
//                        mp ? mp->name : tr("~no sound~"));

//       trackLabel->setToolTip(tooltip);

//       QColor bgCol((QRgb)chan->color());
//       QString trackColorName = bgCol.name();
//       int val = bgCol.value();

//       QString trackStyle = QString(".QLabel {"
//                  "border: 2px solid black;"
//                  "background: %1;"
//                  "color: %2;"
//                  "padding: 6px 0px;"
//              "}").arg(trackColorName, val > 128 ? "black" : "white");

//       trackLabel->setStyleSheet(trackStyle);

//       QColor bgPartCol((QRgb)part->color());
//       QString partColorName = bgPartCol.name();
//       val = bgPartCol.value();

//       //Part header
//       partLabel->setText(part->partName());

//       QString partStyle = QString(".QLabel {"
//                  "border: 2px solid black;"
//                  "background: %1;"
//                  "color: %2;"
//                  "padding: 6px 0px;"
//              "}").arg(partColorName, val > 128 ? "black" : "white");

//       partLabel->setStyleSheet(partStyle);
//       partLabel->setToolTip(tr("This channel is a child of part %1").arg(part->partName()));



//       //Update component colors
//       qreal h, s, v;
//       bgCol.getHsvF(&h, &s, &v);
//       QColor brightCol = QColor::fromHsvF(h, s, 1);
//       panSlider->setScaleValueColor(brightCol);
//       volumeSlider->setHilightColor(brightCol);
//       }

// //---------------------------------------------------------
// //   paintEvent
// //---------------------------------------------------------

// void MixerTrackChannel::paintEvent(QPaintEvent*)
//       {
//       applyStyle();
//       }

// //---------------------------------------------------------
// //   propertyChanged
// //---------------------------------------------------------
// >>>>>>> 3.x:mscore/mixer/mixertrackchannel.cpp

void MixerTrackChannel::propertyChanged(Channel::Prop property)
      {
      update();
      }


void MixerTrackChannel::stripVolumeSliderMoved(int proposedValue)
      {
      takeSelection();
      MixerOptions* options = Mixer::getOptions();

      int acceptedValue;

      if (options->secondaryModeOn()) {
            switch (options->secondarySlider()) {
                  case MixerOptions::MixerSecondarySlider::Pan:
                        acceptedValue = mixerTrackItem()->setPan(proposedValue);
                        break;
                  case MixerOptions::MixerSecondarySlider::Reverb:
                        acceptedValue = mixerTrackItem()->setReverb(proposedValue);
                        break;
                  case MixerOptions::MixerSecondarySlider::Chorus:
                        acceptedValue = mixerTrackItem()->setChorus(proposedValue);
                        break;
            }
      }
      else {
            acceptedValue = mixerTrackItem()->setVolume(proposedValue);
      }

      if (acceptedValue != proposedValue)
      trackSlider->setValue(acceptedValue);
      }


void MixerTrackChannel::stripSoloToggled(bool val)
      {
      mixerTrackItem()->setSolo(val);
      takeSelection();
      }


void MixerTrackChannel::stripMuteToggled(bool val)
      {
      mixerTrackItem()->setMute(val);
      takeSelection();
      }

void MixerTrackChannel::trackColorEdited(QColor color)
      {
      mixerTrackItem()->setColor(color.rgb());
      }


}

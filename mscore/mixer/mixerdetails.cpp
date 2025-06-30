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

#include "mixerdetails.h"

#include "mixer.h"
#include "mixertreerow.h"
#include "mixeroptions.h"
#include "mixertreewidget.h"
#include "libmscore/score.h"
#include "libmscore/part.h"
#include "libmscore/instrument.h"   // ChannelListener
#include "libmscore/undo.h"

namespace Ms {

MixerDetails::MixerDetails(Mixer *mixer) : QWidget(mixer)
      {
      setupUi(this);

      selectedMixerTreeRow = nullptr;
      _mixer = mixer;

      mutePerVoiceGrid = new QGridLayout();
      mutePerVoiceGrid->setContentsMargins(0, 0, 0, 0);
      mutePerVoiceGrid->setSpacing(7);

      connect(partName, &QLineEdit::editingFinished, [&](){
            controlEventParam p{};
            p.s = partName->text();
            _mixer->controlUiEventListener("MixerDetails", "partName", p);
            });
      connect(channelName, &QLineEdit::editingFinished, [&](){
            controlEventParam p{};
            p.s = channelName->text();
            _mixer->controlUiEventListener("MixerDetails", "channelName", p);
            });
      connect(volumeSpinBox, &QSpinBox::valueChanged, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "volumeSpinBox", p);
            });
      connect(panSpinBox, &QSpinBox::valueChanged, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "panSpinBox", p);
            });
      connect(checkBoxSolo, &QCheckBox::toggled, [&](bool x){
            controlEventParam p{};
            p.b = x;
            _mixer->controlUiEventListener("MixerDetails", "checkBoxSolo", p);
            });
      connect(checkBoxMute, &QCheckBox::toggled, [&](bool x){
            controlEventParam p{};
            p.b = x;
            _mixer->controlUiEventListener("MixerDetails", "checkBoxMute", p);
            });
      connect(patchCombo, &QComboBox::activated, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "patchCombo", p);
            });
      connect(drumkitCheck, &QCheckBox::toggled, [&](bool x){
            controlEventParam p{};
            p.b = x;
            _mixer->controlUiEventListener("MixerDetails", "drumkitCheck", p);
            });
      connect(portSpinBox, &QSpinBox::valueChanged, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "portSpinBox", p);
            });
      connect(channelSpinBox, &QSpinBox::valueChanged, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "channelSpinBox", p);
            });
      connect(reverbSlider, &Ms::MixerSlider::valueChanged, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "reverbSlider", p);
            });
      connect(reverbSpinBox, &QSpinBox::valueChanged, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "reverbSpinBox", p);
            });
      connect(chorusSlider, &Ms::MixerSlider::valueChanged, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "chorusSlider", p);
            });
      connect(chorusSpinBox, &QSpinBox::valueChanged, [&](int x){
            controlEventParam p{};
            p.i = x;
            _mixer->controlUiEventListener("MixerDetails", "chorusSpinBox", p);
            });

      }
void MixerDetails::blockSignals(bool x) {
      if (partName) partName->blockSignals(x);
      if (channelName) channelName->blockSignals(x);
      if (volumeSpinBox) volumeSpinBox->blockSignals(x);
      if (panSpinBox) panSpinBox->blockSignals(x);
      if (checkBoxSolo) checkBoxSolo->blockSignals(x);
      if (checkBoxMute) checkBoxMute->blockSignals(x);
      if (patchCombo) patchCombo->blockSignals(x);
      if (drumkitCheck) drumkitCheck->blockSignals(x);
      if (portSpinBox) portSpinBox->blockSignals(x);
      if (channelSpinBox) channelSpinBox->blockSignals(x);
      if (reverbSlider) reverbSlider->blockSignals(x);
      if (reverbSpinBox) reverbSpinBox->blockSignals(x);
      if (chorusSlider) chorusSlider->blockSignals(x);
      if (chorusSpinBox) chorusSpinBox->blockSignals(x);
      }
void MixerDetails::view(DetailsViewPayload p) {
      if (p.updateVis) {
            bool showMidiOptions = Mixer::getOptions()->showMidiOptions();
            labelReverb->setVisible(showMidiOptions);
            reverbSlider->setVisible(showMidiOptions);
            reverbSpinBox->setVisible(showMidiOptions);
            labelChorus->setVisible(showMidiOptions);
            chorusSlider->setVisible(showMidiOptions);
            chorusSpinBox->setVisible(showMidiOptions);
            labelMidiPort->setVisible(showMidiOptions);
            portSpinBox->setVisible(showMidiOptions);
            labelMidiChannel->setVisible(showMidiOptions);
            channelSpinBox->setVisible(showMidiOptions);
            }
      adjustSize();
      blockSignals(true);
      if (p.setEnableWidget) setEnabled(p.enableWidget);
      if (p.setAll || p.setPartName) partName->setText(partName);
      if (p.setAll || p.setChannelName) channelName->setText(channelName);
      if (p.setAll || p.setVolumeSpinBox) volumeSpinBox->setValue(volumeSpinBox);
      if (p.setAll || p.setPanSpinBox) panSpinBox->setValue(panSpinBox);
      if (p.setAll || p.setCheckBoxSolo) checkBoxSolo->setChecked(checkBoxSolo);
      if (p.setAll || p.setCheckBoxMute) checkBoxMute->setChecked(checkBoxMute);
      if (p.clearPatchCombo) patchCombo->clear();
      // if (p.setAll || p.setPatchCombo) patchCombo);
      if (p.setAll || p.setDrumkitCheck) drumkitCheck->setChecked(p.drumkitCheck);
      if (p.setAll || p.setPortSpinBox) portSpinBox->setValue(p.portSpinBox);
      if (p.setAll || p.setChannelSpinBox) channelSpinBox->setValue(p.channelSpinBox);
      if (p.setAll || p.setReverbSlider) reverbSlider->setValue(p.reverbSlider);
      if (p.setAll || p.setReverbSpinBox) reverbSpinBox->setValue(p.reverbSpinBox);
      if (p.setAll || p.setChorusSlider) chorusSlider->setValue(p.chorusSlider);
      if (p.setAll || p.setChorusSpinBox) chorusSpinBox->setValue(p.chorusSpinBox);
      blockSignals(false);
      }

QPushButton* MixerDetails::makeMuteButton(int staff, int voice) {

      QPushButton* muteButton = new QPushButton;
      muteButton->setStyleSheet(QString("QPushButton{padding: 2px 4px 2px 4px;}QPushButton:checked{background-color:%1; color: white;}")
                                .arg(MScore::selectColor[voice].name()));
      muteButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      muteButton->setMinimumWidth (20);
      muteButton->setMinimumHeight(20);
      muteButton->setMaximumWidth (20);
      muteButton->setMaximumHeight(20);
      muteButton->setText(QString("%1").arg(voice + 1));
      muteButton->setCheckable(true);

      QString helpfulDescription = QString(tr("Mute Voice #%1 on Staff #%2")).arg(voice + 1).arg(staff + 1);
      muteButton->setObjectName(helpfulDescription);
      muteButton->setToolTip(helpfulDescription);
      muteButton->setAccessibleName(helpfulDescription);
      return muteButton;
}


// voiceMuteButtonToggled - process button toggled (received via MixerVoiceMuteButtonHandler object)
void MixerDetails::voiceMuteButtonToggled(int staffIndex, int voiceIndex, bool shouldMute)      {
      _mixer->togglePartMutedVoice(selectedMixerTreeRow->part(), staffIndex, voiceIndex, shouldMute);
      }

// midiChannelChanged - process signal from either portSpinBox
// or channelSpinBox, i.e. MIDI port or channel change
void MixerDetails::midiChannelOrPortEdited(int)      {
      //TODO: midi code moved - needs more testing
      if (!selectedMixerTreeRow)
            return;

//      Part* part = selectedMixerTreeRow->part();
//      Channel* channel = selectedMixerTreeRow->channel();
//
//      seq->stopNotes(channel->channel());
//      int p =    portSpinBox->value() - 1;
//      int c = channelSpinBox->value() - 1;
//
//      MidiMapping* midiMap = selectedMixerTreeRow->midiMap();
//      part->masterScore()->updateMidiMapping(midiMap->articulation(), part, p, c);
//
//      part->score()->setInstrumentsChanged(true);
//      part->score()->setLayoutAll();
//      seq->initInstruments();
//
//      // Update MIDI Out ports
//      int maxPort = max(p, part->score()->masterScore()->midiPortCount());
//      part->score()->masterScore()->setMidiPortCount(maxPort);
//      if (seq->driver() && (preferences.getBool(PREF_IO_JACK_USEJACKMIDI) || preferences.getBool(PREF_IO_ALSA_USEALSAAUDIO)))
//            seq->driver()->updateOutPortCount(maxPort + 1);
      }

}


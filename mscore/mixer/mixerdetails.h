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

#ifndef __MIXERDETAILS_H__
#define __MIXERDETAILS_H__

#include "ui_mixerdetails.h"

#include "libmscore/instrument.h"   // ChannelListener

namespace Ms {
class Mixer;
class MixerTreeRow;

struct DetailsViewPayload{
      QList<QTreeWidgetItem*> selectedRows;

      bool updateVis;

      bool setEnableWidget;
      bool enableWidget;

      bool setAll;
      bool setPartName;
      bool setChannelName;
      bool setVolumeSpinBox;
      bool setPanSpinBox;
      bool setCheckBoxSolo;
      bool setCheckBoxMute;
      bool setPatchCombo;
      bool clearPatchCombo;
      bool setDrumkitCheck;
      bool setPortSpinBox;
      bool setChannelSpinBox;
      bool setReverbSlider;
      bool setReverbSpinBox;
      bool setChorusSlider;
      bool setChorusSpinBox;

      QString partName;
      QString channelName;
      int volumeSpinBox;
      int panSpinBox;
      bool checkBoxSolo;
      bool checkBoxMute;
      // QString patchCombo;
      bool drumkitCheck;
      int portSpinBox;
      int channelSpinBox;
      int reverbSlider;
      int reverbSpinBox;
      int chorusSlider;
      int chorusSpinBox;
};
class MixerDetails
      : public QWidget,
        public Ui::MixerDetails,
        public ChannelListener
      {
      Q_OBJECT

      Mixer* _mixer;
      MixerTreeRow* selectedMixerTreeRow = nullptr;
      QGridLayout* mutePerVoiceGrid;
      QList<QWidget*> voiceButtons; // used for dynamically updating tabOrder

      // void updatePartName();
      // void updateChannelName();
      // void updatePatch();
      // void updateVolume();
      // void updatePan();
      // void updateMutePerVoice();
      QPushButton* makeMuteButton(int staff, int voice);
      // void updateMidiChannelAndPort();
      // void updateReverb();
      // void updateChorus();

      void blockSignals(bool);
            
   public slots:
      void midiChannelOrPortEdited(int);

   public:
      MixerDetails(Mixer *mixer);
      void view(DetailsViewPayload);
      // void propertyChanged(Channel::Prop property) override;

      void voiceMuteButtonToggled(int staffIndex, int voiceIndex, bool shouldMute);

      MixerTreeRow* getSelectedMixerTreeRow() { return selectedMixerTreeRow; };
      };

class MixerDetails;

class MixerVoiceMuteButtonHandler : public QObject
      {
      Q_OBJECT

      MixerDetails* mixerDetails;
      int staffIndex;
      int voiceIndex;

   public:
      MixerVoiceMuteButtonHandler(MixerDetails* mixerDetails, int staffIndex, int voiceIndex, QObject* parent = nullptr)
            : QObject(parent),
              mixerDetails(mixerDetails),
              staffIndex(staffIndex),
              voiceIndex(voiceIndex)
            {}

public slots:
      void buttonToggled(bool checked)
            {
            mixerDetails->voiceMuteButtonToggled(staffIndex, voiceIndex, checked);
            }
   };
}
#endif

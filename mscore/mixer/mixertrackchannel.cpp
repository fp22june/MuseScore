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

#include "../libmscore/instrument.h"
#include "../musescore.h"
#include "../audio/midi/msynthesizer.h"     // required for MidiPatch
#include "preferences.h"

#include "mixer.h"
#include "mixertrackitem.h"
#include "mixeroptions.h"
#include "../awl/colorlabel.h"

namespace Ms {

MixerTrackChannel::MixerTrackChannel(MixerTrackItem* item)
      {
      _item = item;

      setupUi(this);
      setupAdditionalUi();
      updateUiControls();
      setupSlotsAndSignals();
      update();
      }

void MixerTrackChannel::setupSlotsAndSignals()
      {
      connect(muteButton,     SIGNAL(toggled(bool)),        SLOT(stripMuteToggled(bool)));
      connect(soloButton,     SIGNAL(toggled(bool)),        SLOT(stripSoloToggled(bool)));
      connect(trackSlider,    SIGNAL(valueChanged(int)),    SLOT(stripVolumeSliderMoved(int)));
      connect(trackSlider,    SIGNAL(sliderPressed()),      SLOT(takeSelection()));
      connect(colorChooser,   SIGNAL(colorChanged(QColor)), SLOT(trackColorEdited(QColor)));
      }

void MixerTrackChannel::takeSelection() { _item->treeWidget()->setCurrentItem(_item); }

void MixerTrackChannel::setupAdditionalUi()
      {
      QString buttonfont = "black";
      switch (preferences.effectiveGlobalStyle()){
            case MuseScoreEffectiveStyleType::LIGHT_FUSION:
                  buttonfont = "black";
                  break;
            case MuseScoreEffectiveStyleType::DARK_FUSION:
                  buttonfont = "white";
                  break;
            }
      QString basicButton("QToolButton{color: " + buttonfont + "; font-weight: bold; border: 1px solid gray;}");
      QString colorTemplate("QToolButton:checked, QToolButton:pressed { color: white; background: %1;}");
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
                        value = _item->getPan();
                        tooltip = tr("Pan: %1");
                        break;
                  case MixerOptions::MixerSecondarySlider::Reverb:
                        value = _item->getReverb();
                        tooltip = tr("Reverb: %1");
                        break;
                  case MixerOptions::MixerSecondarySlider::Chorus:
                        value = _item->getChorus();
                        tooltip = tr("Chorus: %1");
                        break;
                  }
            }
      else {
            value = _item->getVolume();
            tooltip = tr("Volume: %1");
            }


      trackSlider->setValue(value);
      trackSlider->setToolTip(tooltip.arg(QString::number(value)));
      
      muteButton->setChecked(_item->getMute());
      soloButton->setChecked(_item->getSolo());

      int channelColor = _item->color();
      QColor tweakedColor = QColor(channelColor | 0xff000000);
      colorChooser->setColor(tweakedColor);
      //TODO: is it necesary to set the borders every time? and their color
      colorChooser->setStyleSheet(QString("QFrame{background: %1;padding-top: 2px; padding-bottom: 2px; border-radius: 3px;}").arg(tweakedColor.name()));

      setToolTip(_item->detailedToolTip());
      }

void MixerTrackChannel::propertyChanged(Channel::Prop property) { update(); }

void MixerTrackChannel::stripVolumeSliderMoved(int proposedValue)
      {
      takeSelection();
      MixerOptions* options = Mixer::getOptions();

      int acceptedValue;

      if (options->secondaryModeOn()) {
            switch (options->secondarySlider()) {
                  case MixerOptions::MixerSecondarySlider::Pan:
                        acceptedValue = _item->setPan(proposedValue);
                        break;
                  case MixerOptions::MixerSecondarySlider::Reverb:
                        acceptedValue = _item->setReverb(proposedValue);
                        break;
                  case MixerOptions::MixerSecondarySlider::Chorus:
                        acceptedValue = _item->setChorus(proposedValue);
                        break;
            }
      }
      else {
            acceptedValue = _item->setVolume(proposedValue);
      }

      if (acceptedValue != proposedValue)
      trackSlider->setValue(acceptedValue);
      }

void MixerTrackChannel::stripSoloToggled(bool val)
      {
      _item->setSolo(val);
      takeSelection();
      }

void MixerTrackChannel::stripMuteToggled(bool val)
      {
      _item->setMute(val);
      takeSelection();
      }

void MixerTrackChannel::trackColorEdited(QColor color)
      {
      _item->setColor(color.rgb());
      }


}

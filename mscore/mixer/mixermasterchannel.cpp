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

#include "mixermasterchannel.h"

#include "musescore.h"                    // getAction()
#include "audio/midi/msynthesizer.h"     // synti->gain() / setGain()
#include "libmscore/instrument.h"

#include "mixer.h"
#include "mixeroptions.h"
#include "mixertreerow.h"

namespace Ms {

MixerMasterChannel::MixerMasterChannel() {
      setupUi(this);

      playButton->setDefaultAction(getAction("play"));
      loopButton->setDefaultAction(getAction("loop"));

      updateVol(0);

      QString transparentColorLabelStyle = "QLabel{background: none;}"; //colorpicker nonfunc for spacing only
      colorChooser->setStyleSheet(transparentColorLabelStyle);

      connect(trackSlider, SIGNAL(valueChanged(int)), SLOT(masterVolumeSliderMoved(int))); //TODO
      connect(synti, SIGNAL(gainChanged(float)), SLOT(updateVol()));
      }

void MixerMasterChannel::updateVol(float _unused) {
      volumeChanged(synti->gain());
      }

void MixerMasterChannel::updateUiControls() {
      bool showTrackColors = Mixer::getOptions()->showTrackColors();
      colorChooser->setVisible(showTrackColors);
      }

void MixerMasterChannel::volumeChanged(float synthGain) {
      trackSlider->blockSignals(true);
      trackSlider->setDoubleValue(synthGain);
      trackSlider->blockSignals(false);
      }

void MixerMasterChannel::masterVolumeSliderMoved(int value) {
      trackSlider->blockSignals(true);
      float newGain = trackSlider->doubleValue();
      synti->setGain(newGain);
      float n = 20.0;         // from playpanel.h                             //TODO: refactor to non hardcode  
      float mute = 0.0;       // from playpanel.h
      float decibels = (newGain == mute) ? -80.0 : ((n * std::log10(newGain)) - n);
      trackSlider->setToolTip(tr("Volume: %1 dB").arg(QString::number(decibels, 'f', 1)));
      trackSlider->blockSignals(false);
      }
}

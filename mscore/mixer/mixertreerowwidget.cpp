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

#include "mixertreerowwidget.h"

#include "libmscore/instrument.h"
#include "musescore.h"
#include "audio/midi/msynthesizer.h"     // required for MidiPatch
#include "preferences.h"
#include "awl/colorlabel.h"

#include "mixer.h"
#include "mixertreerow.h"
#include "mixeroptions.h"
#include "mixertreewidget.h"

namespace Ms {

MixerTreeRowWidget::MixerTreeRowWidget(MixerTreeRow* item)
      {
      setupUi(this);

      _item = item;
      QString basicButton = QString("QToolButton{color: %1; font-weight: bold; border: 1px solid gray;}")
            .arg(preferences.effectiveGlobalStyle() == MuseScoreEffectiveStyleType::DARK_FUSION ? "white" : "black");
      QString colorTemplate = QString("QToolButton:checked, QToolButton:pressed { color: white; background: %1;}");
      muteButton->setStyleSheet(basicButton + colorTemplate.arg("#D2042D"));
      soloButton->setStyleSheet(basicButton + colorTemplate.arg("green"));

      connect(muteButton, &QToolButton::toggled, [&](bool y) {
            controlEventParam x{};
            x.mixertreerow = _item;
            x.i = y;
            emit controlUiEvent(QString("MixerTreeRowWidget"), QString("stripMuteToggled"), x);
            });
      connect(soloButton, &QToolButton::toggled, [&](bool y) {
            controlEventParam x{};
            x.mixertreerow = _item;
            x.i = y;
            emit controlUiEvent(QString("MixerTreeRowWidget"), QString("stripSoloToggled"), x);
            });
      connect(trackSlider, &QSlider::valueChanged, [&](int y) {
            controlEventParam x{};
            x.mixertreerow = _item;
            x.i = y;
            emit controlUiEvent(QString("MixerTreeRowWidget"), QString("stripSliderMoved"), x);
            });
      connect(colorChooser, &Awl::ColorLabel::colorChanged, [&](QColor y) {
            controlEventParam x{};
            x.mixertreerow = _item;
            x.qc = y;
            emit controlUiEvent(QString("MixerTreeRowWidget"), QString("trackColorEdited"), x );
            });
      }

void MixerTreeRowWidget::view(MTRViewPayload& p) {
      if(trackSlider) trackSlider->blockSignals(true);
      if(muteButton) muteButton->blockSignals(true);
      if(soloButton) soloButton->blockSignals(true);

      if (p.updateCPickerVis == true && colorChooser)
            colorChooser->setVisible(Mixer::getOptions()->showTrackColors());
      if (p.updateColor == true && colorChooser) {
            QColor tweakedColor = QColor(_item->color() | 0xff000000);
            colorChooser->setColor(tweakedColor);
            //colorChooser->setStyleSheet(QString("QFrame{background: %1}").arg(tweakedColor.name())); //weird bug
            }
      if (p.updateSecMode == true && trackSlider) {
            trackSlider->setSecondaryMode(Mixer::getOptions()->secondaryMode());
            trackSlider->setPanMode(Mixer::getOptions()->secondaryMode() && Mixer::getOptions()->secondarySlider() == MixerOptions::MixerSecondarySlider::Pan);
            }
      if (p.setSlider == true && trackSlider) {
            trackSlider->setValue(p.slider);
            }
      if (p.setCol1ToolTip == true && trackSlider) {
            setToolTip(p.toolTip);
            trackSlider->setToolTip(p.toolTip);
            }
      if (p.setMute == true && muteButton)
            muteButton->setChecked(p.mute);
      if (p.setSolo == true && soloButton)
            soloButton->setChecked(p.solo);

      if(trackSlider) trackSlider->blockSignals(false);
      if(muteButton) muteButton->blockSignals(false);
      if(soloButton) soloButton->blockSignals(false);
      }

}
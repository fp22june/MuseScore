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
#include "mixeroptionsbutton.h"
#include "mixeroptions.h"
#include "mixer.h"
namespace Ms {
MixerOptionsButton::MixerOptionsButton(QWidget* parent) : QPushButton (parent) {
      //setCheckable(true);//TODO: in the ui file already?!
      }
void MixerOptionsButton::setupMenu() {
      createMenuActionsAndGroupings();
      adjustMenuActionsInLineWithOptions();
      layoutMenuActions();
      setupSignalsAndSlots();
      }
void MixerOptionsButton::optionChange(bool state) {
      adjustOptionsInLineWithMenu();
      mixer->optionChangeListener();
      }
void MixerOptionsButton::setupSignalsAndSlots() {
      connect(showDetails, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));

      connect(showDetailsToTheSide, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));
      connect(showTrackColors, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));
      connect(showMidiOptions, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));
      connect(showMasterVolume, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));

      connect(makePanSecondarySlider, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));
      connect(makeReverbSecondarySlider, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));
      connect(makeChorusSecondarySlider, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));

      connect(sliderSecModeInvert, SIGNAL(toggled(bool)), this, SLOT(optionChange(bool)));
      }

void MixerOptionsButton::adjustMenuActionsInLineWithOptions() {
      MixerOptions* options = Mixer::getOptions();

      showDetails->setChecked(options->showingDetails());

      showDetailsToTheSide->setChecked(options->showDetailsOnTheSide());
      showMidiOptions->setChecked(options->showMidiOptions());
      showMasterVolume->setChecked(options->showMasterVolume());
      showTrackColors->setChecked(options->showTrackColors());

      makePanSecondarySlider->setChecked(options->secondarySlider() == MixerOptions::MixerSecondarySlider::Pan);
      makeReverbSecondarySlider->setChecked(options->secondarySlider() == MixerOptions::MixerSecondarySlider::Reverb);
      makeChorusSecondarySlider->setChecked(options->secondarySlider() == MixerOptions::MixerSecondarySlider::Chorus);

      sliderSecModeInvert->setChecked(options->sliderSecModeInvert());
      }

void MixerOptionsButton::adjustOptionsInLineWithMenu() {
      MixerOptions* options = Mixer::getOptions();

      options->setShowingDetails(showDetails->isChecked());

      options->setDetailsOnTheSide(showDetailsToTheSide->isChecked());
      options->setMidiOptions(showMidiOptions->isChecked());
      options->setShowMasterVolume(showMasterVolume->isChecked());
      options->setTrackColors(showTrackColors->isChecked());

      MixerOptions::MixerSecondarySlider secondarySlider;
      if (makePanSecondarySlider->isChecked()) {
            secondarySlider = MixerOptions::MixerSecondarySlider::Pan;
            }
      else if (makeReverbSecondarySlider->isChecked()) {
            secondarySlider = MixerOptions::MixerSecondarySlider::Reverb;
            }
      else if (makeChorusSecondarySlider->isChecked()) {
            secondarySlider = MixerOptions::MixerSecondarySlider::Chorus;
            }
      else {
            // something's gone awry - let's just fix it up
            makePanSecondarySlider->setChecked(true);
            makeReverbSecondarySlider->setChecked(false);
            makeChorusSecondarySlider->setChecked(false);
            secondarySlider = MixerOptions::MixerSecondarySlider::Pan;;
            }
      options->setSecondarySlider(secondarySlider);
      options->setsliderSecModeInvert(sliderSecModeInvert->isChecked());
      }

void MixerOptionsButton::layoutMenuActions() {
      menu = new QMenu(this);
      setMenu(menu);
      menu->addSection(tr("Slider while holding Ctrl"));
      menu->addActions({makePanSecondarySlider, makeReverbSecondarySlider, makeChorusSecondarySlider});
      menu->addAction(sliderSecModeInvert);
      menu->addSeparator();
      menu->addAction(showMasterVolume);
      menu->addAction(showDetails);
      menu->addAction(showMidiOptions);
      menu->addAction(showTrackColors);
      menu->addAction(showDetailsToTheSide);
      }

void MixerOptionsButton::createMenuActionsAndGroupings() {
      showDetails = new QAction(tr("Show Details"));
      showDetails->setCheckable(true);
      showDetailsToTheSide = new QAction(tr("Details on the right"));
      showDetailsToTheSide->setCheckable(true);
      showTrackColors = new QAction(tr("Show Track Colors"));
      showTrackColors->setCheckable(true);
      showMasterVolume = new QAction(tr("Show Master Volume"));
      showMasterVolume->setCheckable(true);
      showMidiOptions = new QAction(tr("Show Midi Options"));
      showMidiOptions->setCheckable(true);
      sliderSecModeInvert = new QAction(tr("Switch"));
      sliderSecModeInvert->setCheckable(true);
      makePanSecondarySlider = new QAction(tr("Pan"));
      makePanSecondarySlider->setCheckable(true);
      makeReverbSecondarySlider = new QAction(tr("Reverb"));
      makeReverbSecondarySlider->setCheckable(true);
      makeChorusSecondarySlider = new QAction(tr("Chorus"));
      makeChorusSecondarySlider->setCheckable(true);
      secondarySliderGroup = new QActionGroup(this);
      secondarySliderGroup->addAction(makePanSecondarySlider);
      secondarySliderGroup->addAction(makeReverbSecondarySlider);
      secondarySliderGroup->addAction(makeChorusSecondarySlider);
      }

}

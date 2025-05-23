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

#ifndef __MIXER_H__
#define __MIXER_H__

#include "ui_mixer.h"

namespace Ms {

class Score;
class Channel;
class Part;

class EnablePlayForWidget;
class MixerDetails;
class MidiMapping;
class MixerKeyboardControlFilter;
class MixerContextMenu;
class MixerOptions;
class MixerMasterChannel;

class Mixer : public QDockWidget, public Ui::Mixer
      {
      Q_OBJECT

  public:
      enum class NudgeDirection : int { Up, Down };
      static MixerOptions* options;
      static MixerOptions* getOptions() {return options;};   // UI options, e.g. show/hide track colors, slider modes

  private:
      Score* _score;                            // playback score
      Score* _activeScore;                      // may be a _score itself or its excerpt;
      MixerMasterChannel* _masterChannelWidget; // master volume + play / loop widget
          
      EnablePlayForWidget* enablePlay;

      void setupSlotsAndSignals();
      void showDetails(bool);
      void setPlaybackScore(Score*);

      void enterSecondarySliderMode(bool enter);

      QTimer* ctrlKeyMonitorTimer;
      MixerKeyboardControlFilter* keyboardFilter;     // process key presses for the mixer AND the details panel
      virtual void closeEvent(QCloseEvent*) override;
      virtual void showEvent(QShowEvent*) override;
      virtual void hideEvent(QHideEvent*) override;
      virtual bool eventFilter(QObject*, QEvent*) override;
      virtual void keyPressEvent(QKeyEvent*) override;

   private slots:
      void partOnlyCheckBoxToggled(bool checked);
      void ctrlKeyMonitor();

   public slots:
      void updateTracks();
      void midiPrefsChanged(bool showMidiControls);
      void masterVolumeChanged(double val);
      void synthGainChanged(float val);
     
   signals:
      void closed(bool);

   protected:
      virtual void changeEvent(QEvent *event) override;
      void retranslate(bool firstTime = false);

   public:
      Mixer(QWidget* parent);
      void adjustMasterVolumeTreeWidgetHeaderWidth(QHeaderView*);
      void setScore(Score*);

      MixerDetails* mixerDetails;                                 // TODO: mixerDetails - does it NEED to be public?
      void updateUiOptions();
      void nudgeSecondarySlider(NudgeDirection direction);
      void nudgeMainSlider(NudgeDirection direction);
      int nudge(int currentValue, NudgeDirection direction, int lowerLimit, int upperLimit);

      };

class MixerKeyboardControlFilter : public QObject
      {
      Q_OBJECT
      Mixer* _mixer;
   protected:
      bool eventFilter(QObject *obj, QEvent *event) override;
   public:
      MixerKeyboardControlFilter(Mixer*);
      };

} // namespace Ms
#endif


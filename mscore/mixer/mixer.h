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

// <<<<<<< HEAD:mscore/mixer.h
#include "ui_mixer.h"
// =======
// #include <QWidget>
// #include <QDockWidget>
// #include <QScrollArea>
// #include <QList>

// #include "ui_parteditbase.h"
// #include "ui_mixer.h"
// #include "libmscore/instrument.h"
// #include "enableplayforwidget.h"
// #include "mixertrackgroup.h"
// >>>>>>> 3.x:mscore/mixer/mixer.h

namespace Ms {

class Score;
class Channel;
class Part;

class EnablePlayForWidget;
class MixerDetails;
class MidiMapping;
// <<<<<<< HEAD:mscore/mixer.h
class MixerKeyboardControlFilter;
class MixerContextMenu;
class MixerOptions;
class MixerMasterChannel;
// =======

#if 0
double volumeToUserRange(char v);
double panToUserRange(char v);
double chorusToUserRange(char v);
double reverbToUserRange(char v);

//0 to 100
char userRangeToVolume(double v);
//-180 to 180
char userRangeToPan(double v);
//0 to 100
char userRangeToChorus(double v);
//0 to 100
char userRangeToReverb(double v);
#endif

// >>>>>>> 3.x:mscore/mixer/mixer.h

//---------------------------------------------------------
//   Mixer
//---------------------------------------------------------


class Mixer : public QDockWidget, public Ui::Mixer
      {
      Q_OBJECT

      public:
            enum class NudgeDirection : int { Up, Down };

      Score* _score = nullptr;                        // playback score
      Score* _activeScore = nullptr;                  // may be a _score itself or its excerpt;
      QGridLayout* gridLayout;                        // main layout - used to show/hide & position details panel
      MixerMasterChannel* masterChannelWidget;        // master volume + play / loop widget

      static MixerOptions* options;                   // UI options, e.g. show/hide track colors, slider modes

      EnablePlayForWidget* enablePlay;

      int savedSelectionTopLevelIndex;
      int savedSelectionChildIndex;

      void setupSlotsAndSignals();
      void setupAdditionalUi();
      void showDetails(bool);
      void setPlaybackScore(Score*);

      void enterSecondarySliderMode(bool enter);

      QTimer* shiftKeyMonitorTimer;
      MixerKeyboardControlFilter* keyboardFilter;     // process key presses for the mixer AND the details panel
      virtual void closeEvent(QCloseEvent*) override;
      virtual void showEvent(QShowEvent*) override;
      virtual void hideEvent(QHideEvent*) override;
      virtual bool eventFilter(QObject*, QEvent*) override;
      virtual void keyPressEvent(QKeyEvent*) override;

   private slots:
      void partOnlyCheckBoxToggled(bool checked);
      void shiftKeyMonitor();

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
      void setScore(Score*);

      MixerDetails* mixerDetails;                                 // TODO: mixerDetails - does it NEED to be public?
      void updateUiOptions();
      void nudgeSecondarySlider(NudgeDirection direction);
      void nudgeMainSlider(NudgeDirection direction);
      int nudge(int currentValue, NudgeDirection direction, int lowerLimit, int upperLimit);


      static MixerOptions* getOptions() { return options; };
            
      };

//MARK:- Class MixkerKeyboardControlFilter

class MixerKeyboardControlFilter : public QObject
      {
      Q_OBJECT
      Mixer* mixer;
   protected:
      bool eventFilter(QObject *obj, QEvent *event) override;
      
   public:
      MixerKeyboardControlFilter(Mixer* mixer);
      };


} // namespace Ms
#endif


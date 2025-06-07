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
#include "libmscore/instrument.h"

namespace Ms {
class Score;
// class Channel;
class Part;
class EnablePlayForWidget;
class MidiMapping;
class MixerDetails;
class MixerKeyboardControlFilter;
class MixerContextMenu;
class MixerOptions;
class MixerMasterChannel;

struct controlEventParam { // TODO C++17 std::variant
      int i;
      bool b;
      QColor qc;
      };
struct MixerViewParam {
      bool optionchange;
      bool refreshTree;
      MixerTreeRow* mixertreerow;
      Part* part;
      Instrument* instrument;
      Channel* channel;
      QString item;
      int i;
      bool b;
      QString s;
      };
class Mixer : public QDockWidget, public Ui::Mixer
      {
      Q_OBJECT
  public:
      enum class NudgeDirection : int { Up, Down };
      static MixerOptions* options;
      static MixerOptions* getOptions() {return options;};
  private:
      const int PANSLIDERSHIFT = 64;
      Score* _score;         // playback score
      Score* _activeScore;   // _score or excerpt
  public:                                       //TODO
      MixerDetails* mixerDetails;
  private:
      EnablePlayForWidget* enablePlay;
      MixerMasterChannel* masterChannelWidget;
      //back
      void setSliderSecMode(bool);
      //front
      void showDetails(bool);
      void retranslate(bool firstTime = false);
      MixerKeyboardControlFilter* keyboardFilter; // shortcuts
      virtual void closeEvent(QCloseEvent*) override;
      virtual void showEvent(QShowEvent*) override;
      virtual void hideEvent(QHideEvent*) override;
      virtual bool eventFilter(QObject*, QEvent*) override;
      virtual void keyPressEvent(QKeyEvent*) override;
      virtual void keyReleaseEvent(QKeyEvent*) override;
      virtual void changeEvent(QEvent *event) override;
   private slots:
      void partOnlyCheckBoxToggled(bool checked);
   public slots:
      void controlUiEventListener(QString, MixerTreeRow*, QString, controlEventParam);
      void channelPropChangeEventListener(Channel*, Channel::Prop);
      void updateChannelViewAutoProp(Channel*);
      void updateChannelView(Channel*, Channel::Prop);
      void soundFontChangeListener();
      void playbackScoreChangeListener();
      void treeHeaderWidthChangeListener(QHeaderView*);
      void midiPrefsChanged(bool showMidiControls); // entry Preferences TODO
   signals:
      void closed(bool);
   public:
      Mixer(QWidget* parent);
      void setScore(Score*);  // entry musescore.cpp
      void view(MixerViewParam& m);

      void optionChangeListener();
      //getter
      QString getPartName(Part*);
      QString getInstrumentName(Instrument* instrument);
      QString getChannelName(Channel*);
      int getChannelVolume(Channel*);
      int getChannelChorus(Channel*);
      int getChannelReverb(Channel*);
      int getChannelPan(Channel*);
      bool getChannelMute(Channel*);
      bool getChannelSolo(Channel*);
      int getChannelMidiChannel(Channel*);
      int getChannelMidiPort(Channel*);
      QList<QList<bool>> getPartMutedVoices(Part*);
      int getPartColor(Part*);
      int getIntrumentColor(Instrument*);
      int getChannelColor(Channel*);
      MidiMapping* getMidiMap(Channel*);
      Channel* getPlaybackChannel(Channel*);
      bool getUseDrumset(Channel*);
      //setter
      void setPartName(Part*, QString);
      void setChannelName(Channel*, QString);
      void setChannelVolume(Channel*, int);
      void setChannelMute(Channel*, bool);
      void setChannelSolo(Channel*, bool);
      void setChannelPan(Channel*, int);
      void setChannelChorus(Channel*, int);
      void setChannelReverb(Channel*, int);
      void setPartColor(Part*, int);
      void setIntrumentColor(Part*, Instrument*, int);
      void setChannelColor(Channel*, int);
      void togglePartMutedVoice(Part* part, int staffIndex, int voiceIndex, bool shouldMute);
      void setMidiChannelAndPort(Part*, Channel*, int midiChannel, int midiPort);

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

}
#endif


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

#ifndef __MIXERTRACKITEM_H__
#define __MIXERTRACKITEM_H__

namespace Ms {
class Part;
class Instrument;
class Channel;
class MidiPatch;
class MidiMapping;
class MixerTreeWidget;
class MixerTrackChannel;

enum class MixerItemLevel {
      FOLDER,
      PART,
      INSTRUMENT,
      CHANNEL };
enum class MixerItemPartCat {
      SINGLE_INSTRUMENT_SINGLE_CHANNEL,
      SINGLE_INSTRUMENT_MULTIPLE_CHANNEL,
      MULTIPLE_INSTRUMENT,
      NA
      };

class MixerTrackItem : public QTreeWidgetItem
      {
      
public:
      enum class TrackType { INSTRUMENT, CHANNEL };

private:
      const int PANADJUSTMENT = 64;

      TrackType _trackType;
      Part* _part;
      Instrument* _instrument;
      Channel* _channel;
      MixerTrackChannel* _col1AndChannelBind;

      Channel* playbackChannel(const Channel* channel);

      QList<Channel*> secondaryPlaybackChannels();
      QList<Channel*> playbackChannels();
      QList<Channel*> playbackChannels(Part* part);

      template <class ChannelWriter, class ChannelReader>
      int adjustValue(int proposedValue, ChannelReader reader, ChannelWriter writer);
      template <class ChannelWriter, class ChannelReader>
      int relativeAdjust(int mainSliderDelta, ChannelReader reader, ChannelWriter writer);


      bool isCurrentPatch(const MidiPatch* patch);
      QString adjustedPatchName(const MidiPatch* patch, std::vector<QString> usedNames);

      ~MixerTrackItem();
public:
      MixerTrackItem(
            MixerTreeWidget*,
            TrackType,
            Part*,
            Instrument*,
            Channel*);
      MixerTrackItem(
            MixerTrackItem*,
            TrackType,
            Part*,
            Instrument*,
            Channel*);
      TrackType trackType() { return _trackType; }
      Part* part() { return _part; }
      Instrument* instrument() { return _instrument; }
      Channel* channel() { return _channel; }
      void setCol1AndChannelBind(MixerTrackChannel* col1);

      MidiMapping* midiMap();

      QString detailedToolTip();

      QString getPartName();
      QString getChannelName();    // no setting - the user can't alter this
      int getVolume();
      int getChorus();
      int getReverb();
      int getPan();
      bool getMute();
      bool getSolo();
      int getMidiChannel();
      int getMidiPort();
      int color();

      void setPartName(QString string);
      int setVolume(int value);
      int setPan(int value);
      int setChorus(int value);
      int setReverb(int value);
      void setMute(bool value);
      void setSolo(bool value);
      void resetWithVolume(int volume);
      void setMidiChannelAndPort(int channel, int port);
      void setColor(int valueRgb);

      void toggleMutedVoice(int staffIndex, int voiceIndex, bool shouldMute);
      QList<QList<bool>> getMutedVoices();

      void populatePatchCombo(QComboBox* patchCombo);
      void changePatch(int itemIndex, QComboBox* patchCombo);
      bool getUseDrumset();
      void setUseDrumset(bool useDrumset);

      };

}
#endif // __MIXERTRACKITEM_H__
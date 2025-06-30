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

#ifndef __MIXERTREEROW_H__
#define __MIXERTREEROW_H__

namespace Ms {
class Part;
class Instrument;
class Channel;
struct MidiPatch;
class MidiMapping;
class MixerTreeWidget;
class MixerTreeRowWidget;
class MixerFolderUser;
struct MTRViewPayload;
enum class FPIC {
      FOLDER,
      PART,
      INSTRUMENT,
      CHANNEL };
enum class AdditionFilterFlag {
      SINGLE_INSTRUMENT_SINGLE_CHANNEL,
      SINGLE_INSTRUMENT_MULTIPLE_CHANNEL,
      MULTIPLE_INSTRUMENT,
      NA
      };
class MixerTreeRow : public QTreeWidgetItem
      {
public:
      enum class ItemType { PART, INSTRUMENT, CHANNEL };
private:
      ItemType _itemType;
      MixerFolderUser* _folder;
      Part* _part;
      Instrument* _instrument;
      Channel* _channel;
      MixerTreeRowWidget* _col1widget;
public:
      MixerTreeRow(
            MixerTreeWidget*,
            ItemType,
            // MixerFolderUser* folder,
            Part*,
            Instrument*,
            Channel*);
      MixerTreeRow(
            MixerTreeRow*,
            ItemType,
            // MixerFolderUser* folder,
            Part*,
            Instrument*,
            Channel*);
      // void commoninit();
      void setCol1Widget(MixerTreeRowWidget* w) { _col1widget = w; };
      MixerTreeRow* child(int childIndex) { return static_cast<MixerTreeRow*>(__super::child(childIndex)); };
      MixerTreeRow* parent() { return static_cast<MixerTreeRow*>(__super::parent()); };

      ItemType itemType() { return _itemType; }
      // MixerFolderUser* folder() { return _folder; }
      Part* part() { return _part; }
      Instrument* instrument() { return _instrument; }
      Channel* channel() { return _channel; }

      void view(MTRViewPayload&);

      QString tooltiptext();

      void populatePatchCombo(QComboBox* patchCombo);

      };

}
#endif
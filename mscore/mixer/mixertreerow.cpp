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

#include "mixertreerow.h"

#include <QComboBox>

#include "musescore.h"                    // synti
#include "libmscore/score.h"
#include "libmscore/part.h"
#include "libmscore/instrument.h"
#include "libmscore/undo.h"
#include "seq.h"
#include "audio/midi/msynthesizer.h"     // synti
#include "audio/midi/midipatch.h"
#include "preferences.h"

#include "mixer.h"
#include "mixeroptions.h"
#include "mixertreerowwidget.h"
#include "mixercomp.h"

namespace Ms {
class MixerTreeWidget;
MixerTreeRow::MixerTreeRow(
      MixerTreeWidget* parent, //MixerTreeWidget parent
      ItemType itemType,
      // MixerFolderUser* folder,
      Part* part,
      Instrument* instrument,
      Channel* channel)
      : QTreeWidgetItem(parent, QTreeWidgetItem::UserType+1)
      {
      _itemType = itemType;
      // _folder = folder;
      _part = part;
      _instrument = instrument;
      _channel = channel;
      _col1widget = nullptr;
      }

MixerTreeRow::MixerTreeRow(
      MixerTreeRow* parent, //MixerTreeRow parent
      ItemType itemType,
      // MixerFolderUser* folder,
      Part* part,
      Instrument* instrument,
      Channel* channel)
      : QTreeWidgetItem(parent, QTreeWidgetItem::UserType+1)
      {
      _itemType = itemType;
      // _folder = folder;
      _part = part;
      _instrument = instrument;
      _channel = channel;
      _col1widget = nullptr;
      }

void MixerTreeRow::view(MTRViewPayload& p) {
      if (p.setName == true) setText(0, p.name);
      if (p.setCol0ToolTip == true) setToolTip(0, p.toolTip);
      if (_col1widget) _col1widget->view(p);
      }

void MixerTreeRow::populatePatchCombo(QComboBox* patchCombo) {
      return;
      // patchCombo->clear();

      // // Order by program number instead of bank, so similar instruments
      // // appear next to each other, but ordered primarily by soundfont
      // std::map<int, std::map<int, std::vector<const MidiPatch*>>> orderedPatchList;
      // for (const MidiPatch* patch : synti->getPatchInfo())
      //       orderedPatchList[patch->sfid][patch->prog].push_back(patch);

      // std::vector<QString> usedNames;
      // int patchIndex = 0;
      // for (auto const& soundfont : orderedPatchList) {
      //       for (auto const& pn : soundfont.second) {
      //             for (const MidiPatch* patch : pn.second) {
      //                   if (patch->drum == getUseDrumset() || patch->synti != "Fluid") {                              
                              // QString patchName = patch->name;
                              // if (std::find(usedNames.begin(), usedNames.end(), patchName) != usedNames.end()) {
                              //       QString addNum = QString(" (%1)").arg(patch->sfid);
                              //       patchName.append(addNum);
                              //       }
                              // else {
                              //       usedNames.push_back(patch->name);
                              //       }
                              // bool verbose = false;
                              // if (verbose) {
                              //       return patchName.append(QString(" %1 %2; Bank: %3; Prog: %4; SF: %5")
                              //             .arg(patch->synti)
                              //             .arg(patch->drum ? "🥁" : "🎶")
                              //             .arg(patch->bank)
                              //             .arg(patch->prog)
                              //             .arg(patch->sfid));
                              //       }
      //                         patchCombo->addItem(patchName, QVariant::fromValue<void*>((void*)patch));
                              // if (   patch->synti == _channel->synti(
                              //     && patch->bank == _channel->bank()
                              //     && patch->prog == _channel->program())
      //                               patchIndex = patchCombo->count() - 1;
      //                         }
      //                   }
      //             }
      //       }
      // patchCombo->setCurrentIndex(patchIndex);
      }


}
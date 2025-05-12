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


#ifndef __MIXERTREEWIDGET__
#define __MIXERTREEWIDGET__


#include "libmscore/score.h"
#include "mixertrackitem.h"

namespace Ms {

class MixerTreeWidget : public QTreeWidget
      {
      Q_OBJECT

      int savedSelectionTopLevelIndex;
      int savedSelectionChildIndex;
      void setupSlotsAndSignals();
      QTreeWidget* masterChannelTreeWidget;
      void populateTree(Score* score);
      
      bool anyToExpand();     // used to turn on / off expand all menu
      bool anyToCollapse();   // used to turn on / off collapse all menu
      void resetAll();
      void resetAllSettingVolume(int volume);
      void resetItem(int volume);

private slots:
      void adjustHeaderWidths();
      void itemChanged(QTreeWidgetItem* treeWidgetItem, int column);
      void itemCollapsedOrExpanded(QTreeWidgetItem* item);
      void selectedItemChanged();

public:
      explicit MixerTreeWidget(QWidget *parent = nullptr);
      void saveTreeSelection();
      void restoreTreeSelection();
      void setSecondaryMode(bool secondaryMode);
      void updateSliders();
      void updateHeaders();
      void setScore(Score* score);
      void setMasterChannelTreeWidget(QTreeWidget* masterChannelTreeWidget);

signals:
      void selectedTrackChanged(MixerTrackItem* trackItem);
      void headersResized();

      };

} // namespace Ms
#endif /* __MIXERTREEWIDGET__ */

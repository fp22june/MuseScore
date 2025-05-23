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

#include "mixertrackitem.h" //enums

namespace Ms {
class Score;
class Part;
class Instrument;
class Channel;
class MixerTrackItem;

class MixerTreeWidget : public QTreeWidget
      {
      Q_OBJECT

      int _savedSelectionTopLevelIndex;
      int _savedSelectionChildIndex;
      QTreeWidgetItem* _draggedItem;
      Score* _score;

      void addCol1(MixerTrackItem*, Channel*);
      MixerTrackItem* trackItemFolder(QString foldername, MixerTrackItem* parentItem, int sortOrder, QList<MixerTrackItem*> folders);
      MixerTrackItem* trackItem(
            MixerItemLevel,
            MixerItemPartCat,
            Channel*,
            Instrument*,
            Part*,
            MixerTrackItem*,
            int,
            QList<MixerTrackItem*>);
      void populateTree(Score* score);

      void resetAll();
      void resetAllSettingVolume(int volume);

      void dragEnterEvent(QDragEnterEvent* event) override;
      void dragMoveEvent(QDragMoveEvent* event) override;
      void dropEvent(QDropEvent* event) override;

  private slots:
      void itemCollapsedOrExpanded(MixerTrackItem* item);
      void selectedItemChanged();

  public slots:
      void selectTreeItem(MixerTrackItem* item);

  public:
      MixerTreeWidget(QWidget* parent);
      void emitHeaderWidthChange();
      void saveTreeSelection();
      void restoreTreeSelection();
      void setSecondaryMode(bool secondaryMode);
      void updateSliders();
      void updateHeaders();
      void setScore(Score* score);

      void updateTree();

  signals:
      void selectedTrackChanged(MixerTrackItem* trackItem);
      void headerWidthChanged(QHeaderView*);

      };

} // namespace Ms
#endif /* __MIXERTREEWIDGET__ */

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

#include "mixertreerow.h" //enums

namespace Ms {
class Score;
class Part;
class Instrument;
class Channel;
class MixerTreeRow;
class Mixer;
class MixerFolderUser;
struct MTRViewPayload;
struct MTViewPayload{
      bool updateHeaders;
      bool refreshTree;
      Score* score;

      MTRViewPayload* rowPayload;
      bool all;
      MixerFolderUser* folder;
      Part* part;
      Instrument* instrument;
      Channel* channel;
};
class MixerTreeWidget : public QTreeWidget
      {
      Q_OBJECT

      Mixer* _mixer;
      int _savedSelectionTopLevelIndex;
      int _savedSelectionChildIndex;
      QTreeWidgetItem* _draggedItem;
      Score* _score;

      void addCol1(MixerTreeRow*, Channel*);
      // MixerTreeRow* treeRowFolder(QString foldername, MixerTreeRow* parentItem, int sortOrder, QList<MixerTreeRow*>& folders);
      MixerTreeRow* treeRow(
            FPIC,
            AdditionFilterFlag,
            Channel*,
            Instrument*,
            Part*,
            MixerTreeRow*,
            int);

      void dragEnterEvent(QDragEnterEvent* event) override;
      void dragMoveEvent(QDragMoveEvent* event) override;
      void dropEvent(QDropEvent* event) override;

      void requestSliderUpdate(MixerTreeRow*);
      void arRecursive(MixerTreeRow* rr);
      void rowViewRecursive(MixerTreeRow*, MTViewPayload&);
  private slots:
      void itemCollapsedOrExpanded(MixerTreeRow* item);
      void selectedItemChanged();
  public slots:
      void selectTreeItem(MixerTreeRow* item);
  public:
      MixerTreeWidget(QWidget* parent);
      void setMixer(Mixer*);
      MixerTreeRow* topLevelItem(int);

      void view(MTViewPayload&);
      void allRowSendSliderUpdateRequest();

      void saveTreeSelection();
      void restoreTreeSelection();
  signals:
      void selectedTrackChanged(MixerTreeRow* treeRow);
      void headerWidthChanged(QHeaderView*);
      };
}
#endif

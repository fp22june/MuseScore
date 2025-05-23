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

#include "mixertreewidget.h"

#include <QTreeWidget>

#include "libmscore/score.h"
#include "libmscore/part.h"

#include "mixer.h"
#include "mixeroptions.h"
#include "mixertrackitem.h"
#include "mixertrackchannel.h"

#define MIXERTREE_INVALID_INDEX -1

namespace Ms {

MixerTreeWidget::MixerTreeWidget(QWidget* parent)
      : QTreeWidget(parent)
      {
      _savedSelectionTopLevelIndex = MIXERTREE_INVALID_INDEX;
      _savedSelectionChildIndex = MIXERTREE_INVALID_INDEX;

      updateHeaders();
      header()->setSectionResizeMode(0, QHeaderView::Interactive);
      header()->setSectionResizeMode(1, QHeaderView::Fixed);
      emitHeaderWidthChange();
      connect(header(), &QHeaderView::geometriesChanged, this, &MixerTreeWidget::emitHeaderWidthChange);
      connect(header(), &QHeaderView::sectionResized, this, &MixerTreeWidget::emitHeaderWidthChange);

      connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(selectedItemChanged()));
      connect(this, SIGNAL(itemExpanded(MixerTrackItem*)), SLOT(itemCollapsedOrExpanded(MixerTrackItem*)));
      connect(this, SIGNAL(itemCollapsed(MixerTrackItem*)), SLOT(itemCollapsedOrExpanded(MixerTrackItem*)));
      // connect(this, SIGNAL(itemChanged(MixerTrackItem*, int)), SLOT(itemChanged(MixerTrackItem*, int)));
      }

void MixerTreeWidget::emitHeaderWidthChange() {
      emit headerWidthChanged(header());
      }

// main entry/update point
void MixerTreeWidget::setScore(Score* score) {
      _score = nullptr;
      clear();
      if (score) {
            _score = score;
            populateTree(score);
            }
      if (_savedSelectionTopLevelIndex == MIXERTREE_INVALID_INDEX && topLevelItemCount() > 0) {
            setCurrentItem(this->itemAt(0,0));
            }
      restoreTreeSelection();
      }

void MixerTreeWidget::updateTree() {
      if (!_score)
            return;
      saveTreeSelection();
      clear();
      populateTree(_score);
      restoreTreeSelection();
      }

void MixerTreeWidget::addCol1(MixerTrackItem* item, Channel* channel) {
      MixerTrackChannel* col1 = new MixerTrackChannel(item);
      item->setCol1AndChannelBind(col1);
      channel->addListener(col1); // channel change triggers propertyChanged
      setItemWidget(item, 1, col1); // ownership
      }

MixerTrackItem* MixerTreeWidget::trackItemFolder(QString foldername, MixerTrackItem* parentItem, int sortOrder, QList<MixerTrackItem*> folders) {
      MixerTrackItem* folder;
      for (const auto &f : folders) {
            if (f->text(0) == foldername) {
                  folder = f;
                  break;
                  }
            }
      if (!folder) {
            folder = trackItem(
                  MixerItemLevel::FOLDER,
                  MixerItemPartCat::NA,
                  nullptr,
                  nullptr,
                  nullptr,
                  parentItem,
                  sortOrder,
                  folders);
            }
      else {
            if (sortOrder < folder->text(2).toInt())
                  folder->setText(2, QString::number(sortOrder));
            if (parentItem) 
                  parentItem->sortChildren(2, Qt::AscendingOrder);
            else
                  sortItems(2, Qt::AscendingOrder);
            }
      return folder;
      }

// Part(not MusescorePart), Instrument(>1 if score contains InstrumentChange items), Channel
//       PIC => C
//       PICC => CC
//       PICIC => ICIC
//       PICCICC => ICCICC

// selective
MixerTrackItem* MixerTreeWidget::trackItem(
      MixerItemLevel level,
      MixerItemPartCat category,
      Channel* channel,
      Instrument* instrument,
      Part* part,
      MixerTrackItem* parentItem,
      int sortOrder,
      QList<MixerTrackItem*> folders
      ) {
      MixerTrackItem* item = nullptr;
      if(instrument && !instrument->mixerFolder().isEmpty()) trackItemFolder(instrument->mixerFolder(), parentItem, sortOrder, folders);
      switch (category) {
            case MixerItemPartCat::SINGLE_INSTRUMENT_SINGLE_CHANNEL:
                  switch (level) {
                        case MixerItemLevel::CHANNEL:
                              item = new MixerTrackItem(this, MixerTrackItem::TrackType::CHANNEL, part, instrument, channel);
                              item->setText(0, part->partName());
                              item->setToolTip(0, part->partName());
                              break;
                        }
                  break;
            case MixerItemPartCat::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL:
            case MixerItemPartCat::MULTIPLE_INSTRUMENT:
                  switch (level) {
                        case MixerItemLevel::INSTRUMENT:
                              item = new MixerTrackItem(this, MixerTrackItem::TrackType::INSTRUMENT, part, instrument, channel); //TODO
                              item->setText(0, part->partName());
                              item->setToolTip(0, part->partName());                              
                              break;
                        case MixerItemLevel::CHANNEL:
                              item = new MixerTrackItem(parentItem, MixerTrackItem::TrackType::CHANNEL, part, instrument, channel);
                              item->setText(0, channel->name());
                              item->setToolTip(0, QString("%1 - %2").arg(part->partName()).arg(channel->name()));
                              break;
                        }
                  break;
            }
      if (item) {
            item->setText(2, QString::number(sortOrder));
            if (parentItem) {
                  parentItem->addChild(item);
                  parentItem->sortChildren(2, Qt::AscendingOrder);
                  }
            else {
                  addTopLevelItem(item);
                  sortItems(2, Qt::AscendingOrder);
                  }
            addCol1(item, channel);
            switch (level) {
                  case MixerItemLevel::FOLDER:
                        item->setFlags(QFlag(Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled));
                        break;
                  default:
                        item->setFlags(QFlag(Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled));
                  }
            }
      return item;
      }

// exhausive
void MixerTreeWidget::populateTree(Score* score) {
      int sortOrder = 0;
      QList<MixerTrackItem*> folders;
      for (Part* localPart : score->parts()) {
            sortOrder += 1000;
            Part* part = localPart->masterPart();
            // no addTrackItem ( MixerItemLevel::PART
            const InstrumentList* ils = part->instruments();
            for (auto &il : *ils) {
                  sortOrder += 10;
                  Instrument* instrument = il.second;
                  MixerTrackItem* widgetInstrument = trackItem(
                        MixerItemLevel::INSTRUMENT,
                          1 < ils->size()                   ? MixerItemPartCat::MULTIPLE_INSTRUMENT
                        : instrument->channel().size() <= 1 ? MixerItemPartCat::SINGLE_INSTRUMENT_SINGLE_CHANNEL
                        :                                     MixerItemPartCat::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL,
                        instrument->playbackChannel(0, score->masterScore()),
                        instrument,
                        part,
                        nullptr,
                        sortOrder,
                        folders);
                  for (int i = 0; i < instrument->channel().size(); ++i) {
                        sortOrder += 1;
                        trackItem(
                              MixerItemLevel::CHANNEL,
                                1 < ils->size()                   ? MixerItemPartCat::MULTIPLE_INSTRUMENT
                              : instrument->channel().size() <= 1 ? MixerItemPartCat::SINGLE_INSTRUMENT_SINGLE_CHANNEL
                              :                                     MixerItemPartCat::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL,
                              instrument->playbackChannel(i, score->masterScore()),
                              instrument,
                              part,
                              widgetInstrument,
                              sortOrder,
                              folders);
                        }
                  if(widgetInstrument) widgetInstrument->setExpanded(part->isExpanded()); //TODO part.h  instrument expanded
                  }
            }
      }

void MixerTreeWidget::dragEnterEvent(QDragEnterEvent* event) {
      if (indexAt(event->pos()).column() != 0)
            return;
      qInfo("drag row %s",qPrintable(QString::number(indexAt(event->pos()).row())));
      for(const auto &x :selectedIndexes())
            qInfo("selected %s",qPrintable(QString::number(x.row())));
      // _draggedItem = itemAt(event->pos());
      QTreeView::dragEnterEvent(event);
      }

void MixerTreeWidget::dragMoveEvent(QDragMoveEvent* event) {
    setDropIndicatorShown(true);
    QTreeView::dragMoveEvent(event);
}

void MixerTreeWidget::dropEvent(QDropEvent* event) {
      qInfo("drop %s",qPrintable(QString::number(indexAt(event->pos()).row())));
      //qInfo("%s",qPrintable(QString(event->source()==this?"true":"false" )));
      // insertTopLevelItem(indexAt(event->pos()).row(), _draggedItem);
      QString dropInd;
      DropIndicatorPosition dropIndicator = dropIndicatorPosition();
      switch (dropIndicator) {
            case QAbstractItemView::AboveItem:
            dropInd = "AboveItem";
            break;
            case QAbstractItemView::BelowItem:
            dropInd = "BelowItem";
            break;
            case QAbstractItemView::OnItem:
            dropInd = "OnItem";
            break;
            case QAbstractItemView::OnViewport:
            dropInd = "OnViewport";
            break;
            }
      qInfo("drop %s",qPrintable(dropInd));
      setDropIndicatorShown(false);
      // QTreeView::dropEvent(event);
}

void MixerTreeWidget::selectTreeItem(MixerTrackItem* item) { setCurrentItem(item); }

// also called directly by updateTracks (while signals are disabled)
void MixerTreeWidget::selectedItemChanged() {
      if (topLevelItemCount() == 0 || !currentItem()) {
            emit selectedTrackChanged(nullptr);
            return;
            }
      emit selectedTrackChanged(static_cast<MixerTrackItem*>(currentItem()));
      }

void MixerTreeWidget::updateHeaders() {
      QString secondary = tr("Volume");
      if (Mixer::getOptions()->secondaryModeOn()) {
            switch (Mixer::getOptions()->secondarySlider()) {
                  case MixerOptions::MixerSecondarySlider::Pan:
                        secondary = tr("Pan");
                        break;
                  case MixerOptions::MixerSecondarySlider::Reverb:
                        secondary = tr("Reverb");
                        break;
                  case MixerOptions::MixerSecondarySlider::Chorus:
                        secondary = tr("Chorus");
                  }
            }
      setHeaderLabels({tr("Name"), secondary});
      }

void MixerTreeWidget::setSecondaryMode(bool secondaryMode) {
      updateSliders();
      updateHeaders();
      }

void MixerTreeWidget::updateSliders() {
      // secondary slider style and track colors in the main mixer
      for (int topLevelIndex = 0; topLevelIndex < topLevelItemCount(); topLevelIndex++) {
            MixerTrackItem* topLevelItem = static_cast<MixerTrackItem*>(this->topLevelItem(topLevelIndex));
            MixerTrackChannel* itemWidget = static_cast<MixerTrackChannel*>(this->itemWidget(topLevelItem, 1));
            itemWidget->updateUiControls();

            for (int childIndex = 0; childIndex < topLevelItem->childCount(); childIndex++) {
                  MixerTrackItem* childItem = static_cast<MixerTrackItem*>(topLevelItem->child(childIndex));
                  MixerTrackChannel* itemWidget = static_cast<MixerTrackChannel*>(this->itemWidget(childItem, 1));
                  itemWidget->updateUiControls();
                  }
            }
      }

void MixerTreeWidget::resetAll() { resetAllSettingVolume(64); }

// a different approach would be to ask the SCORE/EXCERPT to do this
// or to build a model, rather than relying on the TreeWidget's default
// model and apply the operation there - but it's marginal
void MixerTreeWidget::resetAllSettingVolume(int volume) {
      //TODO: write the traverse the tree code once and then
      // get the traverse as an array to which we apply operations
      // but THIS may be the only case, in which cas, it's OK
      for (int itemIndex = 0; itemIndex < topLevelItemCount(); itemIndex++) {
            MixerTrackItem* item = static_cast<MixerTrackItem*>(topLevelItem(itemIndex));
            item->resetWithVolume(volume);
            for (int itemIndex = 0; itemIndex < item->childCount(); itemIndex++) {
                  item->resetWithVolume(volume);
                  }
            }
      }

void MixerTreeWidget::itemCollapsedOrExpanded(MixerTrackItem* item) {
      if (item->trackType() == MixerTrackItem::TrackType::INSTRUMENT) {
            item->part()->setExpanded(item->isExpanded());
            }
      }

//TODO rebase jojo 3.x mixer

void MixerTreeWidget::restoreTreeSelection() {
      int topLevel = _savedSelectionTopLevelIndex;
      _savedSelectionTopLevelIndex = MIXERTREE_INVALID_INDEX;   // indicates no selection currently saved
      // there are no items, so we can't select one
      if (topLevelItemCount() == 0) {
            emit selectedTrackChanged(nullptr);
            return;
      }
      blockSignals(true);
      if (topLevel == MIXERTREE_INVALID_INDEX || topLevel < 0 || topLevel >=  topLevelItemCount()) {
            // nothing saved or the saved row is out of range - go to the top of the tree
            setCurrentItem(this->itemAt(0,0));
            }
      else {
            MixerTrackItem* itemOrItsParent = static_cast<MixerTrackItem*>(topLevelItem(topLevel));
            if (!itemOrItsParent) {
                  // the saved row is out of range - go to the top of the tree
                  setCurrentItem(this->itemAt(0,0));
                 }
            else {
                  if (_savedSelectionChildIndex == MIXERTREE_INVALID_INDEX
                      || _savedSelectionChildIndex >= itemOrItsParent->childCount()) {
                        // the child row is off, fall back to parent
                        setCurrentItem(itemOrItsParent);
                        }
                  else {
                        // the saved index is valid, so use it
                        setCurrentItem(itemOrItsParent->child(_savedSelectionChildIndex));
                        }
                  }
            }
      blockSignals(false);
      emit selectedTrackChanged(static_cast<MixerTrackItem*>(currentItem()));
      }

void MixerTreeWidget::saveTreeSelection() {
      MixerTrackItem* item = static_cast<MixerTrackItem*>(currentItem());
      if (!item) {
            _savedSelectionTopLevelIndex = MIXERTREE_INVALID_INDEX;
            return;
            }
      _savedSelectionTopLevelIndex = indexOfTopLevelItem(item);
      if (_savedSelectionTopLevelIndex != MIXERTREE_INVALID_INDEX) {
            // current selection is a top level item
            _savedSelectionChildIndex = MIXERTREE_INVALID_INDEX;
            return;
            }
      MixerTrackItem* parentOfCurrentItem = static_cast<MixerTrackItem*>(currentItem()->parent());
      _savedSelectionTopLevelIndex = indexOfTopLevelItem(parentOfCurrentItem);
      _savedSelectionChildIndex = parentOfCurrentItem->indexOfChild(item);
      }

} // namespace Ms
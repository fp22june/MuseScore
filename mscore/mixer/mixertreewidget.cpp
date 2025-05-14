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
#include "mixertrackitem.h"
#include "mixertrackchannel.h"
#include "mixer.h"
#include "mixeroptions.h"

#include "../libmscore/score.h"
#include "../libmscore/part.h"

#include <QTreeWidget>

#define MIXERTREE_INVALID_INDEX -1

namespace Ms {

MixerTreeWidget::MixerTreeWidget(QWidget *parent) :
      QTreeWidget(parent), savedSelectionTopLevelIndex (MIXERTREE_INVALID_INDEX), savedSelectionChildIndex(MIXERTREE_INVALID_INDEX), masterChannelTreeWidget(nullptr)
      {
      Score* _score = nullptr;

      setSelectionMode(QAbstractItemView::ExtendedSelection);
      setAlternatingRowColors(true);
      setColumnCount(2);
      updateHeaders();

      // setItemDelegateForColumn(1, new NonEditableItemDelegate (this));// col1 non-editable, so that col0 editable only (itemChanged) 

      header()->setSectionResizeMode(0, QHeaderView::Interactive);
      header()->setSectionResizeMode(1, QHeaderView::Fixed);

      setSelectionBehavior(QAbstractItemView::SelectRows);
      adjustHeaderWidths();

      setupSlotsAndSignals();
      }

void MixerTreeWidget::setScore(Score* score)
      {
      clear();
      _score = score;

      if (score)
            populateTree(score);

      if (savedSelectionTopLevelIndex == MIXERTREE_INVALID_INDEX && topLevelItemCount() > 0) {
            setCurrentItem(this->itemAt(0,0));
            }

     restoreTreeSelection();
      }

// Part(not MusescorePart), Instrument(>1 if score contains InstrumentChange items), Channel
//       PIC => C
//       PICC => CC
//       PICIC => ICIC
//       PICCICC => ICCICC
MixerTrackItem* MixerTreeWidget::addTrackItem(MixerItemLevel level, MixerItemPartCat category, Channel* channel, Instrument* instrument, Part* part, MixerTrackItem* parentItem)
      {
      MixerTrackItem* item = nullptr;
      MixerTrackChannel* col1 = nullptr;
      switch (category) {
            case MixerItemPartCat::SINGLE_INSTRUMENT_SINGLE_CHANNEL:
                  switch (level) {
                        case MixerItemLevel::CHANNEL:
                              item = new MixerTrackItem(MixerTrackItem::TrackType::CHANNEL, part, instrument, channel);
                              addTopLevelItem(item);
                              item->setText(0, part->partName());
                              item->setToolTip(0, part->partName());

                              col1 = new MixerTrackChannel(item);
                              item->setCol1AndChannelBind(col1);
                              channel->addListener(col1); // channel change triggers propertyChanged
                              setItemWidget(item, 1, col1); // ownership
                              break;
                        }
                  break;
            case MixerItemPartCat::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL:
            case MixerItemPartCat::MULTIPLE_INSTRUMENT:
                  switch (level) {
                        case MixerItemLevel::INSTRUMENT:
                              item = new MixerTrackItem(MixerTrackItem::TrackType::PART, part, instrument, channel); //TODO
                              addTopLevelItem(item);
                              item->setText(0, part->partName());
                              item->setToolTip(0, part->partName());

                              col1 = new MixerTrackChannel(item);
                              item->setCol1AndChannelBind(col1);
                              channel->addListener(col1); // channel change triggers propertyChanged
                              setItemWidget(item, 1, col1); // ownership
                              break;
                        case MixerItemLevel::CHANNEL:
                              item = new MixerTrackItem(MixerTrackItem::TrackType::CHANNEL, part, instrument, channel);
                              parentItem->addChild(item);
                              item->setText(0, channel->name());
                              item->setToolTip(0, QString("%1 - %2").arg(part->partName()).arg(channel->name()));

                              col1 = new MixerTrackChannel(item);
                              item->setCol1AndChannelBind(col1);
                              channel->addListener(col1); // channel change triggers propertyChanged
                              setItemWidget(item, 1, col1); // ownership
                              break;
                        }
                  break;
            }
      if (item) item->setFlags(QFlag(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
      return item;
      }

void MixerTreeWidget::populateTree(Score* score) {
      for (Part* localPart : score->parts()) {
            Part* part = localPart->masterPart();
            // no addTrackItem ( MixerItemLevel::PART
            const InstrumentList* ils = part->instruments();
            for (auto &il : *ils) {
                  Instrument* instrument = il.second;
                  MixerTrackItem* widgetInstrument = addTrackItem(
                        MixerItemLevel::INSTRUMENT,
                          1 < ils->size()                   ? MixerItemPartCat::MULTIPLE_INSTRUMENT
                        : instrument->channel().size() <= 1 ? MixerItemPartCat::SINGLE_INSTRUMENT_SINGLE_CHANNEL
                        :                                     MixerItemPartCat::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL,
                        instrument->playbackChannel(0, score->masterScore()),
                        instrument,
                        part,
                        nullptr);
                  for (int i = 0; i < instrument->channel().size(); ++i) {
                        addTrackItem(
                              MixerItemLevel::CHANNEL,
                                1 < ils->size()                   ? MixerItemPartCat::MULTIPLE_INSTRUMENT
                              : instrument->channel().size() <= 1 ? MixerItemPartCat::SINGLE_INSTRUMENT_SINGLE_CHANNEL
                              :                                     MixerItemPartCat::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL,
                              instrument->playbackChannel(i, score->masterScore()),
                              instrument,
                              part,
                              widgetInstrument);
                        }
                  // widgetInstrument->setExpanded(part->isExpanded()); //TODO part.h  instrument not part
                  };
            }
      }

void MixerTreeWidget::selectTreeItem(MixerTrackItem* item) {
      setCurrentItem(item);
      }

// - listen for changes to current item so that the details view can be updated
// also called directly by updateTracks (while signals are disabled)
void MixerTreeWidget::selectedItemChanged()
      {
      if (topLevelItemCount() == 0 || !currentItem()) {
            emit selectedTrackChanged(nullptr);
            return;
            }
      emit selectedTrackChanged(static_cast<MixerTrackItem*>(currentItem()));
      }

void MixerTreeWidget::updateHeaders() {

      if (!Mixer::getOptions()->secondaryModeOn()) {
            setHeaderLabels({tr("Name"), tr("Volume")});
            return;
            }

      QString secondary;

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

      setHeaderLabels({tr("Name"), secondary});
      }

void MixerTreeWidget::setupSlotsAndSignals()
      {
      connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(selectedItemChanged()));

      connect(header(), SIGNAL(geometriesChanged()), SLOT(adjustHeaderWidths()));
      connect(header(), &QHeaderView::sectionResized, this, &MixerTreeWidget::adjustHeaderWidths);
      connect(this, SIGNAL(itemExpanded(MixerTrackItem*)), SLOT(itemCollapsedOrExpanded(MixerTrackItem*)));
      connect(this, SIGNAL(itemCollapsed(MixerTrackItem*)), SLOT(itemCollapsedOrExpanded(MixerTrackItem*)));
      // connect(this, SIGNAL(itemChanged(MixerTrackItem*, int)), SLOT(itemChanged(MixerTrackItem*, int)));

      }

void MixerTreeWidget::setSecondaryMode(bool secondaryMode)
      {
      updateSliders();
      updateHeaders();
      }

void MixerTreeWidget::updateSliders()
      {
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

void MixerTreeWidget::adjustHeaderWidths()
      {
      if (masterChannelTreeWidget) {
            masterChannelTreeWidget->header()->resizeSection(0, header()->sectionSize(0));
            masterChannelTreeWidget->header()->resizeSection(1, header()->sectionSize(1));
            }
      }

void MixerTreeWidget::setMasterChannelTreeWidget(QTreeWidget* masterChannelTreeWidget)
      {
      this->masterChannelTreeWidget = masterChannelTreeWidget;
      }
      
void MixerTreeWidget::resetAll()
      {
      resetAllSettingVolume(64);
      }
 

// a different approach would be to ask the SCORE/EXCERPT to do this
// or to build a model, rather than relying on the TreeWidget's default
// model and apply the operation there - but it's marginal
void MixerTreeWidget::resetAllSettingVolume(int volume)
      {
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

bool MixerTreeWidget::anyToExpand()
      {
      // if any item has children and is not expanded
      for (int itemIndex = 0; itemIndex < topLevelItemCount(); itemIndex++) {
            QTreeWidgetItem* item = topLevelItem(itemIndex);
            if (item->childCount() > 0 && !item->isExpanded())
                  return true;
            }
      return false;
      }
      
      
bool MixerTreeWidget::anyToCollapse()
      {
      // if any items have children and are expanded
      for (int itemIndex = 0; itemIndex < topLevelItemCount(); itemIndex++) {
            QTreeWidgetItem* item = topLevelItem(itemIndex);
            if (item->childCount() > 0 && item->isExpanded())
                  return true;
            }
      return false;
      }

void MixerTreeWidget::itemCollapsedOrExpanded(MixerTrackItem* item) {

      if (item && item->isPart()) {
            item->part()->setExpanded(item->isExpanded());
            }
      }

// Used to save the item currently selected in the tree when performing operations
// such as changing the patch. The way changing patches is implemented is that it
// triggers a new setScore() method on the mixer which, in turn, and of necessity,
// forces the channel strips to be built again from scratch. Not clear patch changes
// have to do this, but, currently, they do. This works around that.



void MixerTreeWidget::restoreTreeSelection()
      {
      int topLevel = savedSelectionTopLevelIndex;
      savedSelectionTopLevelIndex = MIXERTREE_INVALID_INDEX;   // indicates no selection currently saved

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
;                 }
            else {
                  if (savedSelectionChildIndex == MIXERTREE_INVALID_INDEX
                      || savedSelectionChildIndex >= itemOrItsParent->childCount()) {
                        // the child row is off, fall back to parent
                        setCurrentItem(itemOrItsParent);
                        }
                  else {
                        // the saved index is valid, so use it
                        setCurrentItem(itemOrItsParent->child(savedSelectionChildIndex));
                        }
                  }
            }

      blockSignals(false);
      emit selectedTrackChanged(static_cast<MixerTrackItem*>(currentItem()));
      }



void MixerTreeWidget::saveTreeSelection()
      {
      MixerTrackItem* item = static_cast<MixerTrackItem*>(currentItem());

      if (!item) {
            savedSelectionTopLevelIndex = MIXERTREE_INVALID_INDEX;
            return;
            }

      savedSelectionTopLevelIndex = indexOfTopLevelItem(item);
      if (savedSelectionTopLevelIndex != MIXERTREE_INVALID_INDEX) {
            // current selection is a top level item
            savedSelectionChildIndex = MIXERTREE_INVALID_INDEX;
            return;
            }

      MixerTrackItem* parentOfCurrentItem = static_cast<MixerTrackItem*>(currentItem()->parent());
      savedSelectionTopLevelIndex = indexOfTopLevelItem(parentOfCurrentItem);
      savedSelectionChildIndex = parentOfCurrentItem->indexOfChild(item);
      }


} // namespace Ms

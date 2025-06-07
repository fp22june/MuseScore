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
#include "mixertreerow.h"
#include "mixertreerowwidget.h"
#include "mixercomp.h"
#include "mixerchannellistener.h"

#define MIXERTREE_INVALID_INDEX -1

namespace Ms {

MixerTreeWidget::MixerTreeWidget(QWidget* parent)
      : QTreeWidget(parent)
      {
      _mixer = nullptr;
      _savedSelectionTopLevelIndex = MIXERTREE_INVALID_INDEX;
      _savedSelectionChildIndex = MIXERTREE_INVALID_INDEX;
      MTViewPayload p{};
      p.updateHeaders = true;
      view(p);
      header()->setSectionResizeMode(0, QHeaderView::Interactive);
      header()->setSectionResizeMode(1, QHeaderView::Fixed);
      connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(selectedItemChanged()));
      connect(this, SIGNAL(itemExpanded(MixerTreeRow*)), SLOT(itemCollapsedOrExpanded(MixerTreeRow*)));
      connect(this, SIGNAL(itemCollapsed(MixerTreeRow*)), SLOT(itemCollapsedOrExpanded(MixerTreeRow*)));
      // connect(this, SIGNAL(itemChanged(MixerTreeRow*, int)), SLOT(itemChanged(MixerTreeRow*, int)));
      //connect(this, &MixerTreeWidget::itemExpanded, &MixerTreeWidget::itemCollapsedOrExpanded);
      //connect(this, &MixerTreeWidget::itemCollapsed, &MixerTreeWidget::itemCollapsedOrExpanded);
      }
void MixerTreeWidget::setMixer(Mixer* x) {
      if (_mixer) {
            disconnect(header(), nullptr, _mixer, nullptr);
            }
      _mixer = x; 
      if (_mixer) {
            connect(header(), &QHeaderView::geometriesChanged, _mixer, [&](){_mixer->treeHeaderWidthChangeListener(header());});
            connect(header(), &QHeaderView::sectionResized, _mixer, [&](){_mixer->treeHeaderWidthChangeListener(header());});
            _mixer->treeHeaderWidthChangeListener(header());
            }
      }
MixerTreeRow* MixerTreeWidget::topLevelItem(int topLevelIndex) { return static_cast<MixerTreeRow*>(__super::topLevelItem(topLevelIndex)); }

// treeSecModeUpdate
void MixerTreeWidget::requestSliderUpdate(MixerTreeRow* r) {
      switch (r->itemType()) {
            // case MixerTreeRow::ItemType::FOLDER:
            //       emit updateChannelViewAutoProp(channel)
            //       break;
            // case MixerTreeRow::ItemType::PART:
            //       emit updateChannelViewAutoProp(channel)
            //       break;
            // case MixerTreeRow::ItemType::INSTRUMENT:
            //       emit updateChannelViewAutoProp(channel)
            //       break;
            case MixerTreeRow::ItemType::CHANNEL:
                  _mixer->updateChannelViewAutoProp(r->channel());
                  break;
            }
      }
void MixerTreeWidget::arRecursive(MixerTreeRow* rr) { 
      MixerTreeRow* r = nullptr;
      for (int childIndex = 0; childIndex < rr->childCount(); childIndex++) {
            r = rr->child(childIndex);
            requestSliderUpdate(r);
            arRecursive(r);
            }
      }
void MixerTreeWidget::allRowSendSliderUpdateRequest() { 
      MixerTreeRow* r = nullptr;
      for (int topLevelIndex = 0; topLevelIndex < topLevelItemCount(); topLevelIndex++) {
            r = topLevelItem(topLevelIndex);
            requestSliderUpdate(r);
            arRecursive(r);
            }
      }

//view
// Part(not MusescorePart), Instrument(>1 if score contains InstrumentChange items), Channel
//       PIC => (P=I=C)
//       PICC => (P=I) C C
//       PICIC => PICIC
//       PICCICC => PICCICC
void MixerTreeWidget::rowViewRecursive(MixerTreeRow* rr, MTViewPayload& p) {
      MixerTreeRow* r = nullptr;
      for (int childIndex = 0; childIndex < rr->childCount(); childIndex++) {
            r = rr->child(childIndex);
            if (     p.all == true
                  || (p.channel && p.channel == r->channel())
                  || (p.instrument && p.instrument == r->instrument())
                  || (p.part && p.part == r->part())
                  //|| (p.folder && p.folder == r->folder()) 
                  )
                  r->view(*p.rowPayload);
            rowViewRecursive(r, p);
            }
      }
void MixerTreeWidget::view(MTViewPayload& p) {
      if (p.rowPayload) {
            MixerTreeRow* r = nullptr;
            for (int topLevelIndex = 0; topLevelIndex < topLevelItemCount(); topLevelIndex++) {
                  r = topLevelItem(topLevelIndex);
                  if (     p.all == true
                        || (p.channel && p.channel == r->channel())
                        || (p.instrument && p.instrument == r->instrument())
                        || (p.part && p.part == r->part())
                        //|| (p.folder && p.folder == r->folder()) 
                        )
                        r->view(*p.rowPayload);
                  rowViewRecursive(r, p);
                  }
            }
      if (p.updateHeaders || p.refreshTree) {
            QString h = tr("Volume");
            if (Mixer::getOptions()->secondaryMode()) {
                  switch (Mixer::getOptions()->secondarySlider()) {
                        case MixerOptions::MixerSecondarySlider::Pan:
                              h = tr("Pan");
                              break;
                        case MixerOptions::MixerSecondarySlider::Reverb:
                              h = tr("Reverb");
                              break;
                        case MixerOptions::MixerSecondarySlider::Chorus:
                              h = tr("Chorus");
                        }
                  }
            setHeaderLabels({tr("Name"), h});
            }
      if (p.refreshTree) {
            clear();
            if (!p.score) return;
            int sortOrder = 0;
            for (MixerFolder* mf : p.score->mixerFolders()) {
                  sortOrder += 1;
                  treeRow(
                        FPIC::FOLDER,
                        AdditionFilterFlag::NA,
                        nullptr, //channel,
                        nullptr, //instrument,
                        nullptr, //part,
                        nullptr, //parent
                        sortOrder);
                  }
            sortOrder = 0;
            for (Part* localPart : p.score->parts()) {
                  sortOrder += 1000;
                  Part* part = localPart->masterPart();
                  const InstrumentList* ils = part->instruments();
                  MixerTreeRow* widgetPart = treeRow(
                        FPIC::PART,
                          1 < ils->size()                   ? AdditionFilterFlag::MULTIPLE_INSTRUMENT
                        :                                     AdditionFilterFlag::NA,
                        nullptr, //channel,
                        nullptr, //instrument,
                        part,
                        nullptr, //parent
                        sortOrder);
                  for (auto &il : *ils) {
                        sortOrder += 10;
                        Instrument* instrument = il.second;
                        MixerTreeRow* widgetInstrument = treeRow(
                              FPIC::INSTRUMENT,
                                1 < ils->size()                   ? AdditionFilterFlag::MULTIPLE_INSTRUMENT
                              : instrument->channel().size() <= 1 ? AdditionFilterFlag::SINGLE_INSTRUMENT_SINGLE_CHANNEL
                              :                                     AdditionFilterFlag::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL,
                              instrument->playbackChannel(0, p.score->masterScore()),
                              instrument,
                              part,
                              nullptr, //parent
                              sortOrder);
                        for (int i = 0; i < instrument->channel().size(); ++i) {
                              sortOrder += 1;
                              Channel* c = instrument->playbackChannel(i, p.score->masterScore());
                              treeRow(
                                    FPIC::CHANNEL,
                                      1 < ils->size()                   ? AdditionFilterFlag::MULTIPLE_INSTRUMENT
                                    : instrument->channel().size() <= 1 ? AdditionFilterFlag::SINGLE_INSTRUMENT_SINGLE_CHANNEL
                                    :                                     AdditionFilterFlag::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL,
                                    c,
                                    instrument,
                                    part,
                                    widgetInstrument,
                                    sortOrder);
                              //subscibeChannelPropChange                                     TODO memory man
                              MixerChannelListener* mcl = new MixerChannelListener(c);
                              c->addListener(mcl); // channel change triggers propertyChanged(){ emit channelPropChangeEvent }
                              connect(mcl, &MixerChannelListener::channelPropChangeEvent, _mixer, &Mixer::channelPropChangeEventListener);
                              }
                        if(widgetInstrument) widgetInstrument->setExpanded(part->expand()); //TODO part.h  instrument expanded
                        }
                  }
            }
      }
void MixerTreeWidget::addCol1(MixerTreeRow* item, Channel* channel) {
      MixerTreeRowWidget* col1 = new MixerTreeRowWidget(item);
      item->setCol1Widget(col1);
      setItemWidget(item, 1, col1); // ownership
      //init
      MTRViewPayload x{};
      x.updateCPickerVis = true;
      x.updateColor = true;
      x.updateSecMode = true;
      item->view(x);
      requestSliderUpdate(item);
      //
      _mixer->updateChannelViewAutoProp(channel);
      connect(col1, &MixerTreeRowWidget::controlUiEvent, _mixer, &Mixer::controlUiEventListener);
      }
// selective
MixerTreeRow* MixerTreeWidget::treeRow(
      FPIC fpic,
      AdditionFilterFlag laff,
      Channel* channel,
      Instrument* instrument,
      Part* part,
      MixerTreeRow* parentItem,
      int sortOrder
      ) {
      MixerTreeRow* item = nullptr;
      if(fpic == FPIC::FOLDER) {
            // item = new MixerTreeRow(this, MixerTreeRow::ItemType::CHANNEL, nullptr /*folder*/, part, instrument, channel);
            // item->setText(0, _mixer->getPartName(part) );
            // addCol1(item, channel);
            }
      else switch (laff) {
            case AdditionFilterFlag::SINGLE_INSTRUMENT_SINGLE_CHANNEL:
                  switch (fpic) {
                        case FPIC::CHANNEL:
                              item = new MixerTreeRow(this, MixerTreeRow::ItemType::CHANNEL/*, nullptr*/ /*folder*/, part, instrument, channel);
                              item->setText(0, _mixer->getPartName(part) );
                              addCol1(item, channel);
                              break;
                        }
                  break;
            case AdditionFilterFlag::SINGLE_INSTRUMENT_MULTIPLE_CHANNEL:
                  switch (fpic) {
                        case FPIC::INSTRUMENT:
                              item = new MixerTreeRow(this, MixerTreeRow::ItemType::INSTRUMENT/*, nullptr*/ /*folder*/, part, instrument, channel);
                              item->setText(0, _mixer->getPartName(part) );
                              item->setSizeHint(0, QSize(0, 26)); // row height for without MixerTreeRowWidget
                              break;
                        case FPIC::CHANNEL:
                              item = new MixerTreeRow(parentItem, MixerTreeRow::ItemType::CHANNEL/*, nullptr*/ /*folder*/, part, instrument, channel);
                              item->setText(0, _mixer->getChannelName(channel));
                              addCol1(item, channel);
                              break;
                        }
                  break;
            case AdditionFilterFlag::MULTIPLE_INSTRUMENT:
                  switch (fpic) {
                        case FPIC::PART:
                              item = new MixerTreeRow(this, MixerTreeRow::ItemType::PART/*, nullptr*/ /*folder*/, part, instrument, channel);
                              item->setText(0, _mixer->getPartName(part));
                              item->setSizeHint(0, QSize(0, 26)); // row height for without MixerTreeRowWidget
                              break;
                        case FPIC::INSTRUMENT:
                              item = new MixerTreeRow(this, MixerTreeRow::ItemType::INSTRUMENT/*, nullptr*/ /*folder*/, part, instrument, channel);
                              item->setText(0, _mixer->getPartName(part) + " : " + _mixer->getInstrumentName(instrument) );
                              item->setSizeHint(0, QSize(0, 26)); // row height for without MixerTreeRowWidget
                              break;
                        case FPIC::CHANNEL:
                              item = new MixerTreeRow(parentItem, MixerTreeRow::ItemType::CHANNEL/*, nullptr*/ /*folder*/, part, instrument, channel);
                              item->setText(0, _mixer->getChannelName(channel));
                              addCol1(item, channel);
                              break;
                        }
                  break;
            }
      if (item) {
            //add
            if (parentItem) parentItem->addChild(item);
            else addTopLevelItem(item);
            //sort
            item->setText(2, QString::number(sortOrder));
            if (parentItem) parentItem->sortChildren(2, Qt::AscendingOrder);
            else sortItems(2, Qt::AscendingOrder);
            //ui
            switch (fpic) {
                  case FPIC::FOLDER:
                        item->setFlags(QFlag(Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled));
                        break;
                  default:
                        item->setFlags(QFlag(Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled));
                  }
            }
      return item;
      }

// entry point
// void MixerTreeWidget::updateTree() {
//       qInfo()<<"MixerTreeWidget::updateTree()";
//       if (!_score)
//             return;
//       saveTreeSelection();
//       clear();
//       //refreshTree(_score);
//       restoreTreeSelection();
//       }
//

//
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

void MixerTreeWidget::selectTreeItem(MixerTreeRow* item) { setCurrentItem(item); }

void MixerTreeWidget::selectedItemChanged() {
      if (topLevelItemCount() == 0 || !currentItem()) {
            emit selectedTrackChanged(nullptr);
            return;
            }
      emit selectedTrackChanged(static_cast<MixerTreeRow*>(currentItem()));
      }

void MixerTreeWidget::itemCollapsedOrExpanded(MixerTreeRow* item) {
      if (item->itemType() == MixerTreeRow::ItemType::INSTRUMENT) {
            item->part()->setExpand(item->isExpanded());
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
            MixerTreeRow* itemOrItsParent = static_cast<MixerTreeRow*>(topLevelItem(topLevel));
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
      emit selectedTrackChanged(static_cast<MixerTreeRow*>(currentItem()));
      }

void MixerTreeWidget::saveTreeSelection() {
      MixerTreeRow* item = static_cast<MixerTreeRow*>(currentItem());
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
      MixerTreeRow* parentOfCurrentItem = static_cast<MixerTreeRow*>(currentItem()->parent());
      _savedSelectionTopLevelIndex = indexOfTopLevelItem(parentOfCurrentItem);
      _savedSelectionChildIndex = parentOfCurrentItem->indexOfChild(item);
      }

} // namespace Ms
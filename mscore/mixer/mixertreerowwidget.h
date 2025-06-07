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

#ifndef __MIXERTREEROWWIDGET_H__
#define __MIXERTREEROWWIDGET_H__

#include "ui_mixertreerowwidget.h"

namespace Ms {
class MixerTreeRow;
class MixerTreeWidget;
struct controlEventParam;

struct MTRViewPayload {
      bool setName;
      QString name;
      bool updateCPickerVis;
      bool updateColor;
      bool updateSecMode;
      bool setSlider;
      int slider;
      bool setToolTip;
      QString toolTip;
      bool setMute;
      bool mute;
      bool setSolo;
      bool solo;
      };
class MixerTreeRowWidget
      : public QWidget,
        public Ui::MixerTreeRowWidget
      {
      Q_OBJECT

      MixerTreeRow* _item;
      MixerTreeWidget* _tree;      
  private slots:
      void stripMuteToggled(bool);
      void stripSoloToggled(bool);
      void stripSliderMoved(int);
      void trackColorEdited(QColor);
  public slots:
      void view(MTRViewPayload& payload);
  signals:
      void controlUiEvent(QString, MixerTreeRow*, QString, controlEventParam);
  public:
      MixerTreeRowWidget(MixerTreeRow*);
      };
}
#endif
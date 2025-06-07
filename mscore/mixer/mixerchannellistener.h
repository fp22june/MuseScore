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
#ifndef __MIXERCHANNELLISTENER_H__
#define __MIXERCHANNELLISTENER_H__
#include "libmscore/instrument.h"   // ChannelListener
namespace Ms {
class MixerChannelListener
      : public QObject,
        public ChannelListener
      {
      Q_OBJECT
      Channel* _channel;
  private slots:
      void propertyChanged(Channel::Prop property) override;
  signals:
      void channelPropChangeEvent(Channel*, Channel::Prop);
  public:
      MixerChannelListener(Channel*);
      };

}
#endif
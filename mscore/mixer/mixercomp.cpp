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
#include "mixercomp.h"
#include "libmscore/score.h"
#include "libmscore/xml.h"
#include "libmscore/instrument.h"
namespace Ms {
MixerRow::MixerRow(){
      _color = DEFAULT_COLOR;
      }
bool MixerRow::readProperties(const QStringRef& tag, XmlReader& e) {
      //_color = e.intAttribute("color");
      //_mixerVolume = e.intAttribute("mixerVolume");
      //_mixerPan = e.intAttribute("mixerPan");
      //_mixerSolo = e.intAttribute("mixerSolo");
      //_mixerMute = e.intAttribute("mixerMute");
      if (tag == "color")
            _color = e.readInt();
      else if (tag == "mixerVolume")
            _mixerVolume = e.readInt();
      else if (tag == "mixerPan")
            _mixerPan = e.readInt();
      else if (tag == "mixerSolo")
            _mixerSolo = e.readInt();
      else if (tag == "mixerMute")
            _mixerMute = e.readInt();
      else
            return false;
      return true;
      }
void MixerRow::write(XmlWriter& xml) const {
      if (_color != DEFAULT_COLOR)
            xml.tag("color", _color);
      if (_mixerVolume != Channel::defaultVolume)
            xml.tag("mixerVolume", _mixerVolume);
      if (_mixerPan != 64)
            xml.tag("mixerPan", _mixerPan);
      if (_mixerSolo != false)
            xml.tag("mixerSolo", _mixerSolo);
      if (_mixerMute != false)
            xml.tag("mixerMute", _mixerMute);
      }
MixerDescendant::MixerDescendant(){
      _parentfolder = nullptr;
      }
MixerFolder::MixerFolder(){
      _mixerExpand = false;
      }
MixerFolderUser::MixerFolderUser(Score* score)
      : ScoreElement(score) {
      _folderid = 1 ;
      if (score) {
            for (auto &f : score->mixerFolders()) {
                  if (_folderid <= f->folderid() ) _folderid = f->folderid() + 1;
                  }
            }
      }
bool MixerFolderUser::readProperties(XmlReader& e) {
      _mixerExpand = e.intAttribute("mixerExpand");
      return true;
      }
void MixerFolderUser::read(XmlReader& e) {
      while (e.readNextStartElement()) {
            if (!readProperties(e))
                  e.unknown();
            }
      }
void MixerFolderUser::write(XmlWriter& xml) const {
      xml.stag(this);
      xml.tag("mixerExpand", _mixerExpand);
      xml.etag();
      }

}
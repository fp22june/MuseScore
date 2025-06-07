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
namespace Ms {
MixerColorable::MixerColorable(){
      _color = DEFAULT_COLOR;
      }
MixerDescendant::MixerDescendant(){
      _parentfolder = nullptr;
      }
MixerFolder::MixerFolder(){
      _expand = false;
      _vol = 0;
      _pan = 0;
      _solo = false;
      _mute = false;
      }
MixerFolderUser::MixerFolderUser(Score* score)
      : ScoreElement(score) {
      _folderid = 1 ;
      if (score) {
            for (auto &f : score->mixerFolders()) {
                  if (_folderid <= f->folderid() ) _folderid = f->folderid() + 1;
                  }
            }
      _score = score;
      }
bool MixerFolderUser::readProperties(XmlReader& e) {
      _expand = e.intAttribute("expand");
      _vol = e.doubleAttribute("vol");
      _pan = e.intAttribute("pan");
      _solo = e.intAttribute("solo");
      _mute = e.intAttribute("mute");
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
      xml.tag("expand", _expand);
      xml.tag("vol", _vol);
      xml.tag("pan", _pan);
      xml.tag("solo", _solo);
      xml.tag("mute", _mute);
      xml.etag();
      }

}
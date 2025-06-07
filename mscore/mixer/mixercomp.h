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
#ifndef __MIXERCOMP_H__
#define __MIXERCOMP_H__
#include "libmscore/scoreElement.h"
namespace Ms {
class Score;
class XmlWriter;
class XmlReader;

class MixerFolder;
class MixerColorable {
  protected:
      static const int DEFAULT_COLOR = 0x3399ff;
      int _color;
  public:
      MixerColorable();
      int color() const { return _color; };
      void setColor(int x) { _color = x; };
      };
class MixerDescendant {
  protected:
      MixerFolder* _parentfolder;
  public:
      MixerDescendant();
      MixerFolder* mixerFolder() { return _parentfolder; };
      void setMixerFolder(MixerFolder* x) { _parentfolder = x; };
      };
class MixerFolder { // score->mixerFolders()
  protected:
      bool _expand;
      float _vol; // ratio
      int _pan;
      bool _solo;
      bool _mute;
  public:
      MixerFolder();
      bool expand() const { return _expand; };
      float vol() { return _vol; };
      int pan() { return _pan; };
      bool solo() { return _solo; };
      bool mute() { return _mute; };
      void setExpand(bool x) { _expand = x; };
      void setVol(float x) { _vol = x; };
      void setPan(int x) { _pan = x; };
      void setSolo(bool x) { _solo = x; };
      void setMute(bool x) { _mute = x; };
      };

class MixerFolderUser final : public ScoreElement, public MixerFolder {
      int _folderid;
      Score* _score;
  public:
      MixerFolderUser(Score* score = 0);
      int folderid() const { return _folderid; };
      ElementType type() const override { return ElementType::PART; }; //TODO
      QVariant getProperty(Pid) const override { return 0; };
      bool setProperty(Pid, const QVariant&) override { return true; };
      bool readProperties(XmlReader&);
      void read(XmlReader&);
      void write(XmlWriter& xml) const;
      bool operator==(const MixerFolderUser& x) const { return _folderid == x.folderid(); }
      };
}
#endif
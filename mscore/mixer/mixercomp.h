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
class MixerRow { // Channel, Instrument, Part 
  protected:
      static const int DEFAULT_COLOR = 0x3399ff;
      int _color;
      int _mixerVolume;
      int _mixerPan;
      bool _mixerSolo;
      bool _mixerMute;
  public:
      MixerRow();
      int color() const { return _color; };
      void setColor(int x) { _color = x; };
      int mixerVol() { return _mixerVolume; };
      int mixerPan() { return _mixerPan; };
      bool mixerSolo() { return _mixerSolo; };
      bool mixerMute() { return _mixerMute; };
      void setMixerVol(int x) { _mixerVolume = x; };
      void setMixerPan(int x) { _mixerPan = x; };
      void setMixerSolo(bool x) { _mixerSolo = x; };
      void setMixerMute(bool x) { _mixerMute = x; };

      bool readProperties(const QStringRef&, XmlReader&);
      void write(XmlWriter&) const; 
      };
class MixerDescendant {
  protected:
      MixerFolder* _parentfolder;
  public:
      MixerDescendant();
      MixerFolder* mixerFolder() { return _parentfolder; };
      void setMixerFolder(MixerFolder* x) { _parentfolder = x; };
      };
class MixerFolder { // Part, MixerFolderUser // score->mixerFolders()
  protected:
      bool _mixerExpand;
  public:
      MixerFolder();
      bool expand() const { return _mixerExpand; };
      void setMixerExpand(bool x) { _mixerExpand = x; };
      };

class MixerFolderUser final : public ScoreElement, public MixerFolder {
      int _folderid;
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
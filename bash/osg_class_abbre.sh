#!/bin/bash
OSG_HOME=/usr/local/source/OpenSceneGraph/include/
ctags -f - --excmd=number --sort=no --fields=Ks --kinds-c++=sc --language-force=c++ \
    ${OSG_HOME}osg/* \
    ${OSG_HOME}osgAnimation/* \
    ${OSG_HOME}osgDB/* \
    ${OSG_HOME}osgFX/* \
    ${OSG_HOME}osgGA/* \
    ${OSG_HOME}osgManipulator/* \
    ${OSG_HOME}osgParticle/* \
    ${OSG_HOME}osgPresentation/* \
    ${OSG_HOME}osgQt/* \
    ${OSG_HOME}osgShadow/* \
    ${OSG_HOME}osgSim/* \
    ${OSG_HOME}osgTerrain/* \
    ${OSG_HOME}osgText/* \
    ${OSG_HOME}osgUI/* \
    ${OSG_HOME}osgUtil/* \
    ${OSG_HOME}osgViewer/* \
    ${OSG_HOME}osgVolume/* \
    ${OSG_HOME}osgWidget/* \
 | cut -f 1,5 | \
while  read name scope; do
  if [[ "$scope" =~ class: || "$scope" =~ struct: ]]; then
    continue
  fi

  if [[ "$scope" =~ namespace: ]]; then
    scope=${scope:10}
  else
    continue
  fi
  abbrevation=$(echo "${name//[[:lower:]]/}" | tr '[:upper:]' '[:lower:]' )
  namespaceAbbrevation=o$(echo "${scope//[[:lower:]]/}" | tr '[:upper:]' '[:lower:]' )
  if [[ "$scope" = osgSim ]]; then
    namespaceAbbrevation=oi
  elif [[ "$scope" = osgTerrain ]]; then
    namespaceAbbrevation=or
  elif [[ "$scope" = osgVolume ]]; then
    namespaceAbbrevation=oo
  elif [[ "$scope" == osgPresentation ]]; then
    namespaceAbbrevation=oe
  fi
  # becareful, a-z will match upper case, due to LC_COLLATE
  echo  "$namespaceAbbrevation$abbrevation"  "$scope"::"$name"
done

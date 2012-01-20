#ifndef SOURCEFINDER_H
#define SOURCEFINDER_H

#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>

class SourceFinder
{
public: // functions
   explicit SourceFinder();
   virtual ~SourceFinder();

   QStringList& ignoreDirs();
   QList<QRegExp>& ignoreFileRegexps();

   QFileInfoList findSourceFiles(QString startingDir);

private: // functions

private: // members
   QStringList srcFileFilters_;
   QStringList ignoreDirs_;
   QList<QRegExp> ignoreFileRegexps_;
};

#endif

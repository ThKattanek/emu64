//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tree_widget_mod.h                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef TREE_WIDGET_MOD_H
#define TREE_WIDGET_MOD_H

#include <QTreeWidget>
#include <QItemDelegate>
class TreeWidgetMod : public QTreeWidget
{
    Q_OBJECT
public:
    explicit TreeWidgetMod(QWidget *parent = 0);
protected:
    void drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
signals:

public slots:

};

#endif // TREE_WIDGET_MOD_H

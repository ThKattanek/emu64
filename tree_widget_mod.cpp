//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: tree_widget_mod.cpp                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "tree_widget_mod.h"
#include <QPainter>

TreeWidgetMod::TreeWidgetMod(QWidget *parent) :
    QTreeWidget(parent)
{
}

void TreeWidgetMod::drawRow(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const
{
    QTreeWidget::drawRow(p, opt, idx);
    for (int col = 0; col < columnCount(); ++col)
    {
        QModelIndex s = idx.sibling(idx.row(), col);
        if (s.isValid())
        {
            QRect rect = visualRect(s);
            p->setPen(QPen(QColor(000,000,255,50)));
            p->drawRect(rect);
        }
    }
}

/****************************************************************************
** $Id: qt/examples/themes/wood.cpp   2.3.2   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "wood.h"
#include "qapplication.h"
#include "qpainter.h"
#include "qdrawutil.h" // for now
#include "qpixmap.h" // for now
#include "qpalette.h" // for now
#include "qwidget.h"
#include "qlabel.h"
#include "qimage.h"
#include "qpushbutton.h"
#include "qwidget.h"
#include "qrangecontrol.h"
#include "qscrollbar.h"
#include <limits.h>

/* XPM */
static const char *polish_xpm[] = {
/* width height num_colors chars_per_pixel */
"    96    96      254            2",
/* colors */
".. c #9c4a34",
".# c #a4825c",
".a c #bc5e2c",
".b c #d48432",
".c c #dc9f51",
".d c #bc6e1c",
".e c #d4855d",
".f c #94664c",
".g c #bc714e",
".h c #8c6664",
".i c #d4923c",
".j c #bc8444",
".k c #d49360",
".l c #d4794e",
".m c #ecaf68",
".n c #bc8365",
".o c #d47439",
".p c #a46954",
".q c #dc9f70",
".r c #e48544",
".s c #bc7b51",
".t c #a47761",
".u c #bc7b42",
".v c #a4523c",
".w c #e4945e",
".x c #9c784c",
".y c #d4844a",
".z c #eca053",
".A c #bc614c",
".B c #e4855c",
".C c #bc8350",
".D c #c48e68",
".E c #b16634",
".F c #e49339",
".G c #bc703a",
".H c #bc7c67",
".I c #a45f34",
".J c #cc714d",
".K c #d48c5f",
".L c #a47057",
".M c #cc703a",
".N c #dca674",
".O c #b47859",
".P c #bc6729",
".Q c #d49475",
".R c #d48b4a",
".S c #cc8351",
".T c #cc8466",
".U c #ac6841",
".V c #e4a651",
".W c #e49576",
".X c #d47d31",
".Y c #ac6e4b",
".Z c #c07650",
".0 c #e48c43",
".1 c #e49452",
".2 c #9c745f",
".3 c #e47e54",
".4 c #cc7c4f",
".5 c #cc7c32",
".6 c #b46133",
".7 c #d49a68",
".8 c #d67e4f",
".9 c #bc7643",
"#. c #b47056",
"## c #d48b3a",
"#a c #dc9f5e",
"#b c #e49a60",
"#c c #cc6a31",
"#d c #8c6244",
"#e c #dc9a41",
"#f c #eca753",
"#g c #bc8a58",
"#h c #d48c76",
"#i c #bc693f",
"#j c #bc715d",
"#k c #9c6857",
"#l c #f4b171",
"#m c #bc8a6a",
"#n c #eca16d",
"#o c #a87e58",
"#p c #a4613f",
"#q c #a48569",
"#r c #d4846d",
"#s c #dc935f",
"#t c #c47c50",
"#u c #dc8449",
"#v c #bc6950",
"#w c #cc9678",
"#x c #c4703a",
"#y c #cc7b67",
"#z c #dc8c5e",
"#A c #ac7067",
"#B c #eca86e",
"#C c #b4786d",
"#D c #dc8c4a",
"#E c #b46842",
"#F c #d47c41",
"#G c #e48d51",
"#H c #e59a52",
"#I c #9c6e3f",
"#J c #d49351",
"#K c #cc843b",
"#L c #ecb678",
"#M c #9c5a38",
"#N c #d4795c",
"#O c #c47b39",
"#P c #ec9560",
"#Q c #ac764c",
"#R c #c48351",
"#S c #c48e74",
"#T c #cc7650",
"#U c #cc8a84",
"#V c #bc6a5c",
"#W c #e4af74",
"#X c #b46855",
"#Y c #e4a06e",
"#Z c #ac775b",
"#0 c #e48d5d",
"#1 c #c47d65",
"#2 c #cc763f",
"#3 c #b47e5d",
"#4 c #cc8a55",
"#5 c #cc8a67",
"#6 c #bf622f",
"#7 c #dc853b",
"#8 c #e49f4a",
"#9 c #9c505c",
"a. c #8c5644",
"a# c #cc7329",
"aa c #a45a51",
"ab c #b48264",
"ac c #9c7a7c",
"ad c #9c5f4f",
"ae c #b4844c",
"af c #a46749",
"ag c #dca664",
"ah c #b46e1c",
"ai c #c4762c",
"aj c #a45a3c",
"ak c #dc9a74",
"al c #ac7e46",
"am c #ac6a6c",
"an c #eca862",
"ao c #e49a41",
"ap c #e49a78",
"aq c #bc7660",
"ar c #d57e5e",
"as c #9c6e5c",
"at c #ab7e65",
"au c #cc8a44",
"av c #9c6240",
"aw c #bc6244",
"ax c #bc5d3f",
"ay c #e48550",
"az c #eca060",
"aA c #cc7160",
"aB c #cc7c42",
"aC c #b46241",
"aD c #b4726c",
"aE c #eca67f",
"aF c #9c6a3c",
"aG c #94685a",
"aH c #c48240",
"aI c #c48465",
"aJ c #dc7640",
"aK c #cc8f54",
"aL c #e4a76f",
"aM c #c4692e",
"aN c #dc9474",
"aO c #ac6050",
"aP c #b47048",
"aQ c #94614b",
"aR c #ac836c",
"aS c #a47048",
"aT c #b4764a",
"aU c #ec8e5c",
"aV c #dc9a53",
"aW c #cc765e",
"aX c #b48a64",
"aY c #dc9a63",
"aZ c #c47640",
"a0 c #ec9a60",
"a1 c #c48a54",
"a2 c #c48a67",
"a3 c #ac5a3c",
"a4 c #ac8458",
"a5 c #dc855d",
"a6 c #c4714d",
"a7 c #dc9243",
"a8 c #dc794e",
"a9 c #ac6955",
"b. c #cc8f67",
"b# c #ac6032",
"ba c #ac7056",
"bb c #dc7a34",
"bc c #ec9553",
"bd c #dc8d3b",
"be c #e4a060",
"bf c #f4a654",
"bg c #c46842",
"bh c #c46f62",
"bi c #ac613d",
"bj c #dc866c",
"bk c #c4694e",
"bl c #dc7d42",
"bm c #ec8d4f",
"bn c #dc9351",
"bo c #cc9177",
"bp c #c4695f",
"bq c #ecb075",
"br c #e4a75f",
"bs c #d4843c",
"bt c #bc722c",
"bu c #d4936c",
"bv c #d47644",
"bw c #bc7d5c",
"bx c #ac563c",
"by c #e4956c",
"bz c #a47a4c",
"bA c #d48454",
"bB c #bc825c",
"bC c #e49544",
"bD c #bc7044",
"bE c #bc7e74",
"bF c #d48d6c",
"bG c #cc7144",
"bH c #b47864",
"bI c #bc6a34",
"bJ c #d49684",
"bK c #d48b54",
"bL c #cc845c",
"bM c #cc8474",
"bN c #ac684c",
"bO c #cc7d5c",
"bP c #eca27c",
"bQ c #dc946c",
"bR c #c47c5c",
"bS c #dc8554",
"bT c #c47244",
"bU c #dc8c6c",
"bV c #dc8c54",
"bW c #b4684c",
"bX c #cc8344",
"bY c #c47b44",
"bZ c #c4825c",
"b0 c #e4a17c",
"b1 c #ac7a64",
"b2 c #e48c6c",
"b3 c #c47a74",
"b4 c #e49f54",
"b5 c #9c674c",
"b6 c #946764",
"b7 c #c48674",
/* pixels */
"#u#G#G#P#G#G#G.1#G#G.1.1.1.w#G.r#D.1.1.1#D#DbVbV.K.K.K.KbO.Z.Z#TaP.GaT.Z.O.O.O.H.9aP.ZaPaPaPbZbo.i.k#J.k#JbKbnbn#b#sbVbV#G#G.r.ray.r.0#G.0#G.1bc.r.0.0bc.0.0.0.r.1.1.1.1#G#D.0#D.0.0.0bcbcbc.1.0",
"#aagaLbrag#a#a#a#DbVbn#G#0.1#0#Da5#za5.4.J.Jbk#vbt.G#x.9bY.4#tbYbw.saPaP.Ub#af.Y.s.s.Z.saP#E.gbw.U.U.UaObWbWbWaPb#b#.E#t.K.K#z#s#s#sbQ#s#sbn#s#sbn#s.w#s.w#s#sbn.ybV#s#sbV#s#sbQ#aak.7.k.7.k.k.k",
"#..g#.#.#j#.#XbW#Z#o.O.O#3.n.n.Halalalala4aXaXa4#t.u.9#R#5bu.k#5bob7aIaI.nbwbw#m#5aIa2#5#5aIb7#5.DbBbB#3bwbw.C.O#oabab.naI.C#t#R.9#tbY#tbY.ZbY.s#t#t#t#R.sbY.s#tbD.Z.Z#t.9.Z#t.4bBbw.s.saT.9aTaT",
".ybS.ybSbVbVbVbVbl#u#u#ubSaybSay#s#za5#z#z#z.KbA.Qb.aIbZbZbB.g.U.gaP#.aP#.aPaP.O.9aP.g.s.O.ZaP.Z#CaD#Aamamamam#X#3#Z.OaPaPaPaP.saZbY#t#t#tbY#tbY#t#tbLbZ.SbR#t#tbZbRbZbRbR.sbRbR.4aBaB.4bY.4.4.4",
"bKbV#zbKbA.ybK#zbV#z#0bS#0#0#0aya5bA.4.4.4#T#xbgbwaq.O.g.O#j#.aPaCaC#v.g.gaPaP.Z#R#R#t#t#t.Z.s.Z#O#O.5aH#KbY.S#4bD.G#xaZaZ#2.SbVbV#s.wbV#zbSbAa5.e.KbQbQbU#s#z.K#5.K#5.T#4#5.Kb..4bZbL.T.ebL.K.K",
"#A#A#C#C#AaD#CbEatataRb1b1abb1.t#I.f#IaSbz.x.#.##v.g.g#1bR.T#5#hbObO.T.K.K#5.KbubL.4.4#t.4.Z#tbL.e#r.K.K.e.e.T#r.K#z.K#z#sbVbS#zbV#u#ubVbl#F.8.l#2.4.8bAbA.l.4bva6.g#x.g#x#i#i#i.L.Lba#Z#Z#Z#Zba",
"aPaT.u.u.s.C.C.C.SbA.S.4.SbA.8.8#r#rbF#h.TbO.T#h.KbF.e.e.S.S.S.S#u.y.8bA.e.4bA.e.K#zbQ#s.K.KbUbQ.y.R.y.yaB#O#xbT#t.4bLbLbL.4bL.S.4.S.4#T.Za6.Z.JbD.g.Z.g.Z.g.Z.ZaP.g#iaP#i#E#EaC.Obababa.ObH#Z.O",
"#zbSbA.e#za5.e.8bV.KbV.ybAbK#zbK.8.ybV#z.y#F.y#sbKbA.Rbn#sbn#b#b#Y.w#sbV.y.4.lbA.y.y.ybA#T.MbG.4bObO.4.SbLbOaq#..O.Yaf#p#p.I.U.UaT.Z.O.Oaq.O.H.Hb1b1#Z#Z.L#Zat#3bw.s.HbBb7aIaIaIbA.l.4aBbAbA.e.e",
"#DbVbn.w#s.w#sbVbAbSbSbV#D#GbS#0.1#H#H#H#b.1bn.1#4.S#t#t.S.TbL.S.K.K.K.S.SbK.e.S#t.4#t#tbDaPb#b#.U#E.9.S.ebVbS#G.K.K#r.l#y#T.Z.Z#2aB.S.ybS#u.8#F#Kau.y.S.y.y.R.y#z#z#sbn.1bna7bd#F#DbVbVbV#s.w#s",
".1#s.1#s.1bVbS.y.K#z#sbVbVbA.8#FbA.8ar.8.e#r.e#NbKbA.S.S.S.SbLbLaBaBaB#2aB.SbKbAbVbVbV#s.w#YbebQ.KbA.e.KbV.ybl.ybsbsbs#D#u#D#u.R.y#D#DbVbV#s#0.wbebe#b#bbQ.wbn#GbV#G#D#G#D#GbV#D.ybV#sbV#s#s.w#s",
".wbn.w.w#bbQbVbV.4.4.ybKbnbV#z#z#G#D#D#D#u#D.y.R#2aB#2.M#x.M#2#2#z#s.w#sbV#s.w#s.1.w.w#b.w.1#G#G#s#zbVbVbVbSbVbVbd#ubd.r#D#D#D#D.1bC.1.1.1.1#D.1bV#G#0#0#G.w.1a0bC#G.0aybS.Ba5a5bVbVbV.w#z.w#sbV",
"#ubVbVbVbV#GbV#D#s#sbn#b.w.w.1bVb2b2a5#z#z.K#zbA.w#s.w#sbV#s#z#s.w#b.w.w#GbV.w.w.X#Da0a0#G.1bcaz#G#GbS#GbV#ubV#0#z#0#0a5#0#0b2#0.0.r.0.0#u#u#F.o.M.M.M#F#ubV#G.1#b#P.w.w.w#0aya5.y#u.y.ybVbnbVbV",
".wbV#GbV#G#s#G#0.1#G.1.1.1#G#G.0#0#0.w.wby.wbQbn.w.w#G.w.wa0#b#P#u#G#G#0#G#G#G.w#baz#Yaz.1.0#D.0#G#G#G#GbVbl.8blbva8.8.B#z.Ba5#0ay#G#G#z#G#z#z#z#zbU#z#z#zbQbybQ#zbK#z#s#sbn.R.y#2#2aB.8bVbV#0bV",
".w.w.w#z.w.w.w.w#GbV.r.0.0#G#G#Gbda7a7#H#8#8#8#H#u#u#F#u#D#GbV#u#G#G.w.1.w#G.wa0bV#D#DbV.w#baz.w#G#G#G#G#0.BbSa5#u#u#D.1.1bn.0#7#z#0bSbSbSa5#z#z#r.e.e.4.4#2#2bT.4.4.4.S.R.Rbn.i#s.K#zbV#s#0bV.w",
"#u.8#u#u#ubV#GbVby.wbQ.w#b.wbVbl#T#T.l.ear.Ba5.8.w#G#G#G.w.1.1.1.1#G#G.w.1#G#G.w#P.w#G.1.w#P.w#0#D#DbV.w.w#0#0#0.w#b#baVaVbn.1#G.y.y#F#F#2.obv#Fay#z#GbVbV#z.e.e#z#z#zbV#s#s#s#sbQbQ#sbQ.wbV#G#0",
"#0#0#0#G#0#u#ublbDbT.4#4#zbQ.e.e#s#s#s#z.w#0#0aU#DbSbVbV#D#D#D#G.w#G#G.w#P.w.w.w#Gbmbmbmay#u#G.waz#b.w.w#Ga5bl#uaraW#i#i#ia6.4.ebVbnbQ#b.w.w#GbV#u.r.r#G#G#G#0a5.1.wbV#zbSar.J.JbT#x#2.y#F.8#u#G",
"#5.TbL#tbD#i.g.Z.SbA#zbAbKbSbSbA#u#D#G#u#u#u#DbV#GbVay#G#0#G#G.r#D#Gbn.w#b#b#HaY.1bVbV.y#u.y#F#Fbv.o.M#2#2#2aBbG.9#OaB.y#D.1.1.1.w#s.1.w#G#G#G.rbdbda7bdbnbn#sbQ.nbw.s.Z.4.8.8.8.obl#u#zbV#z.K.K",
"#i#x.Z#tbL.K.kbQbAbVbK#u.ybSbV#z#G.w.w#G#D#G#0#G.1#G#D.1.1.1.1.1bVbV#z.yaBaM.M.5.y.y.y#ubV.w#P.w.w.w#b.1.1.w.wby#b.w#b.w#b.w.1.1#u#u#ubVbAbK#z.S.T.TbO.Z#vbga3axbD#xaZ.8bAbAbAbA.e#4bA#ta6.P.6.6",
".S.4bL.e.e.e.ebA.8bSbSbSbVay#0#G.1.1a0.1#ba0#H#bbc#Hbc#Ha0bc.1.0#z.8#T.J.l.ebVbV#G#G#G.w.w.w.w#P.w#b#n#b.1.1.w#n.1.1.1#G#G#D#ublbl#2.4.4.g.Z#ZbN#9#9aa#X.g.ZbOar.Mbvbla5#z#zbA.laPbNbi.U.U#..Zbw",
"bL.e.e.e#zbSbSbl#u#DbS#G#G#0#G#G.1.1.1.1bc#Ha0.1#G.1.1a0#b.1#u#F.e.4#T.8by#b#na0a0#b.w.w.w.w#P#0bVbV.1.1#G#G#u#G.1#G#G#ubS.8.l#T.Z.g#ibW.UbNa9#p.UaP.9.S.ybVbV#Dayay#z#z.e.4bT#i.pad#pbN#.bRaI.T",
".4.e.KbS.8blblbSbSbS#GbV#G#0#G#G#G.0.1.1bcaz#H.1#G.1.w#P.w.w#z.8bK.KbQbQbV#u#D.1#0#G#G#G#G#G.1#P.1.1beazbe.1#G#u#zbK.K.4bO.Z#j#v#A#ka9.YbW.ZbL.4.R.R.ibn#D#u#F#FbS.S.4aqaPbNbi.I#.bWaP.gbObL.8bL",
".K.K#z.e#F.lbv#F#z#GbS#u#u#G.w#G#u#G#G#G.1a0bc.1#P#P#P.w#GbVarar.R#J#HaY.1.1.1#H#H#b.1.1bc.1#P.1.w#b#bazbe.w#zbA#t#t.Z.Z.Z.g#.#.b1ba#..Z.Z.lbS#u.y#u#ubSbS#N.laA#j.ga9#kad#ka9#..g.g#tbO.e.e.ebA",
".l.l.8.y.8bAbAa5bSbS#ubSbS#0.w#G#G#G#G#G.1bc.1bCbm.1.1.0#Dblbv#T#D.R.ybVbe#nazanananbeaz#b#bbc#H.wa0.1#sbV.S.S#t#t.s.Z.sbwaIaIaI#t#t.4bA.lbl#ubl.3a5a5.e.4bh#V#XbN#k#kaG#k.L.OaIbL.e.K.ebA.y.4.8",
"bSbAbAa5.ebA.8.4.8.ybS#z#G.w#0bSay#0bm#G#G.0.1.0bc#H.zbf.zbe.1#z.KbVbSbA.y#D#D.1bebebe#HbeazazazazazbebV.S.4#1bMbZbR.Z.Z.Z#t#tbYbl.o#ubS#Gay#0#uarbl.laB#t.uaT.Oad#kaGb6#ka9.g.Z#t#t.4.4#t.4bK.K",
"aBbs.y.ybA.l.lar.4aBaB.8.y#F#u.wbn#D#D#G.0.0a7.FbdbCbc.1.1.0.1bc.F#8#H.1.ybG.4.ebn#s#s#baY#saYaV#b.w#s#J.SaZbD.sbR#t.S#z#ubs#F.R#D#D.0#G#G#G#G.0#D.R.yaBbDbDaP.g#Zb1.L.fb6.h.hac.I.YaT.u#t.Z.Z#2",
"b3bR#1.4.4.4bX.R.K.KbV.ybl#F#F#F#za5a5.B#z#0#sby.1#G.1.1.1bm#G.1a0a0#H#H#b#b#s.ya#.X.y.ybVbn.w#bbebeaY#sbAbRbZa2au.S.R#s#z.y.e#zbs#ubn.1.1.1#G#D.0#D#D.RbA.4bO.4aPbD#Eb#af.I#Maf.2.x.L.L#Z.O.n#S",
"aObi#Xbw.s.sbY.SbXbK#z#z.w.w.wa0#z#za5bS.l.l.8bVbSbVbS#G#G#G.w#Pbe.w#b#bazbrb4#Hbebebe.1#u#F.5.X#F.y#s#Y.NaN.Q.Q.T.Tbu.w#sbn.1be#sbVbV.y.y.ybn#b#G#G#G#G#0bn#zbSbn#bbn#D.R.RaBbX#3.O.Yaf.I.Ibi#E",
"aVbX.G.6a3aCb3#U.y#z#s#0#z.w.w#s#H#H.1.1.0#G#Hb4bVbV#u#u#u#D.0#D.1.1.1#D#7#D#Hb4bCbc.1.w#b#b#n#nbe.1#u.yaBbX.S.kb4.cb4aYbA#T.4#r.4.K.K#s#s#s#D.y#GaybV.w.w.w.w.w#G.w.w.w#b.1bn.1#P.w.w#0#0aybS#G",
"bnaVaYbeaNarbp.A.P#2#F.y#u#s.w#sb4b4.1.1.1.1#H#H#nbeaz#b#HbnbCa7.zbe.z.1#D#D.w#Baz.z#H.w#P.w.w.wbebeanbrbebn.RbX.4bQb0aL#BaYa7.bbA.4#xaMaZ.ybK.kbA.4.4#F.8.ybA.ybv#u#ubS#sbV#D#zbl.ray#G#G#P.w#P",
".w#0bSbVbV#D#D.1aY#b.w#zbS#FaBaB#x.M.l.8a5byap#bbn.1bebebebebeaza7.0bd#u#ubSbS.BbC#D.0#D#G.w.w.1anazb4bebebebr#B.V.c#D#Oai.S#s.WaE#Y#Y#YaY.kaB#x#4.S.S.4aBbX.y.y.y.K#z.KbV#z.ybV.ybKbKbK.K.K#z.k",
".8.8#u#G.w#HaV.1#s.w#s#0#zbV.wbQ#na0by#0a5bSbvbg.M#F.y#ubSbA#z.k.w.w.w.wa0a0.w.w#H.w#HbV#D#u#G.1.1.1#b.w.1.1#D#DbVakaLaLaLbq#B.VaB.XaB.ybKbQaLaL.q.qaY#a#b.w#b#b#b#b.wbn#s#D.y#D#s#s#s#JbKau.SbA",
"b4#Hbe#baY#s.K.S.y#F.ybSbS.R#zbe#H#H.1#Gbc#b#bbn.w#bbybQ.4#xbga6#T.l#za5bV#G#Gbc.w#b#b.wbV#D.1beby.wbVbV#z#s.w#Y#bbeaY#J#s#Ybe.1#B#B#Bbr#s.RbA.KbKbnaV.w#b.w#Ha0b4anbe#Hbe.1.1#bbC#8bc#Ha0a0#Pbc",
"b4#8#8#H#HbCbC.1b4#b.w.1#P#G#G.0akbVbXaB#2#2.4bAbA.KbAbV#s#b#YaY.1bV.8aZ.GbYaB.S.RaV#abebn.1#8az#b.w.w#b#ba0by.w.1#Hazbebe.1#s#b#ebnbnbnaVaVaYbnbnb4bebe#abe#abebnbnbnbn.ibnbnbn##a7#8#8#ea7beaL",
"#Y.w.w#s.w#0#z#0#D#DbVbVbVbV#bana7#D.1.1#b.1#D#D#u#u#Fa#.o.y#D#D.r.r.w#b#Y#b#b.w.gbkbg#Tar#za5a5####.5.b.Ra7.1bn#HaV#H#Hbn#D#D.ybQbQ#z.K.e.K.e.ea7bnaVa7bnaY#aaL.m#Wbrbr#Ybr#W.m#L#L#L.mbeb4.w#b",
"#sbK.ybS#z#zbSbS.k.k.K.S.SbAbKbn.w.1#D#GbV#GbV.w#G#G#G#u#u#ubl#7.r#G#G#0#zayay.0#Y.wa5bS#zbS.8.M.4.Z#xbka6#TbObO#D.Rbn#s#bbebebebV.R.R.y.R#D#DbVbebe#BbqaLbebn.Rbnbnbebr#BbrbeaV#Da7bran.mbr.1#D",
"anazbean#Banbe#H.y#O#x#x.ybKbK.R.Jbvar.ebja5#N#TbvbS#z#0#0.1#0bV#G#u#u.o#Fbb#7bm#F#7#u#D#Dbn#bbe#s#s#sbn.w#zbVbVbkbpbpbpbhbhbhbhaAaAaWaW#N#r#rbFbs.ybKbn#b#YaLbq#Bbrbebebebr#BbqbeaLbe#Y#B#B#B#B",
"a7#DbCb4azananan#BaL#b#Yby#b#G#ub0aN.e#x#i#xaWbObAa5.e.8.4#2.4.4.w.wbQ#zbSbSbVbV.w#z#zbS.JbGbG.l#D#GbV.1.w.1#G#G#H#8#8#8aoa7#8#8#D#Dbn.1bnbC.1bC#b.w#D#D.y.5bs.y##a7#Hbranbe.1.i#sbn#D.Rbsbs#ubs",
"#b#0.y.8#2#2#2#2#F#u#G.w#0#G#Pazb4b4beb4bnbn##.5#xaB.4.4.4.e#z.K.e.SbA.8bA.ebK.K#zbV.y#ubVbVbVbVar.8ara8ara8a5ar.RbV#zbVbA.y#D#sbSa5a5bV#zbSbVbS#G#DbV#b#b.1bn#bananbeb4b4anananbean#B#B#Bazbebe",
"#0a5bSbV.w#Y#BaE.1#0.wbSay#uay#Gbd.FbCbC#H.zanaz#b#b#b.w#zbA#2#c.P#6#6.M#2.e.K.k#KaB.Xbsbn.1.1#7#Hbna7#D#D#D#D.0#0#zbVbV#u#F#FblbdbCbC.1bC#G.1.1bV#u#D#G#Hbean.manbeanan#B.manan.mbebe.1bV.1.1.1",
".zbCa7#Hazanb4#8#Y.w.y.8#z.w#G#D#G#0#P#0#0#0#uaJ#D#u#ubl#D#0.wb2.w.1#0#0.w#z.4.G#vbhbRbO#rar#N.l#4bL.S.4.4.4.4bLay#D#uay.1#Ha0.w.1.w.1.w#G#s#0.w#G.1.1an#Bazb4b4anazb4b4.zananan#f#f#8#b#H.w.1#b",
"b4#Hbd#Ha7#H.1.0a7#H#b.wbV#s.w#0#D#D#D#D#G#u#u#G#G#G#GbV#u.8.y#ubs#7#G.1.1bn#D.y#xbXbAbAbYbt.Z#RbXbK#s.k.R.S.RbAbl#F.o.o#Fbv.yblbV#u#7#G.1.z.z.z.w.1#G#u#D.0#G#nb4#H.1.1#b.zb4.z#H.1.1.1#Hbe.m#B",
"#Y.w#D.1ay.w.w#Ga7#H#H.w#G#G#G#G#D.wbV#G.1.w#G.1#D#D#ubSbA#u.KbV#s#G#u#u#ubVbn#s.zb4b4#sbO.g#V#X.MaB.yaBaB#2aZ#2.y.y.R.KbQ.KbQbu#zbAbK#s#bbe#Ybea0be#bbn.w#D.y#G.1bC.1.1a0#b.z.z.zbc#H.zbCao#HbC",
".wbS#0.wa0a0#na0azaz#b#H.1#G.0.0.w#P.w#G#0#G#uay#D#DbAbK#z.KbSbK.l.4bv.8#F.l.e.S#ubAbA.RbK.K.K#s.4bAa5#z#z#z#z#z#w#m#3ba.p.p.L.L.ZaZbD#t#R.SaKaKakap#Y#bak#s#zakbn#D#u#D.y#7bs#7#F#F#u#0#0#0by.w",
"bna7anbran#8az#8.1#ba0#bbcbeaza0a0.w#G#G.1bm#G#0.1#z#GbV#z#0#s#G.e#z#z#za5.8bGbgaAbv#T.l#F#Da7a7bQbybQ#z.e#z.ebAblblay.0ay.raJ.obR.s#tbR#t.s.s#tbDaZa6bT.Z.gbT.SbA.S.S.KbQ.q.q#YbqaLbeaV#D#u#D#u",
"auauaL#W#Wbr#L#Wbe#nbe.w.w.w.1a7.1#H.1.0#u#7#7.r.0#ubl#uay#Gay#u#z#za5#u#ubS#za5#D#u#u.8.8.8.8#r.4bYaZ.G.GaZaZaZ#t.SbLbFb.#5bO.Z#t.Z.Z#t.Z#iaC.E.Z#1.g.ga6#ibWbR.C.s#..OaPbNbi.Ua9ajaO#X#v#y#r#h",
"bL#1b.aI.g.I.UaP.GaZ#t.4.SbK#sbQbe#Bbe#b.1#D#D.w#G#DbSbl.8bl#u#F.8aB#F#F#F#ubV.w.ybSbS#u.8.8a5#z.k.K.KbK.KbQaYakbu.K.S.4.4.4.S.e.4.8.4.ebF.Kb.#h.gbR#taq.ZbW#E.gaP.YbNba#.babaaP#3.u.s#R#R.S.KbK",
"a6a6.Z.ebO.T.QaNa2a2aIaT.Ub#.6.EbIaZaZ.4.S.SbA.kakbQ.k.KbF.ebA.y.e.ebSbS#u.8#FbG#r#r#r.e#F.ybsbdbV#zbV.yaBaB#2#2#MavaS#Z.O.O.OaPa6#i#i.ZbRaI.H.n.ZaIaI.T#5aIbRb.b.bLaIbRbR.gbWaP.g#v.g.Z#x.gbT#i",
"#H.1#DaY.ybV.SaB.Y.O.O#.#X.ObBaIbR.T#1#t#1.Z.g.ZaP.U.U#EbW.g.g.g#T#T.4.l.l.8bS.e#F.8#F.l.8arararblbSa5#0bS.BbS#ubV#u#ubl#ublblbl#5bZ.gaP#.babN.pb##E#E#E.Z.g.gaIaB#2.ZbL.T.e.e.K#z.kby#b#b#Y#Baz",
"brbrb4#Hb4.1#D.0.R.y.y#F.yaBbvaB.Z.g.g.g#jaOaxbxaFaS.Y.O.s.s#ta2#Fbla5bSay#ua8bl.y.y.RbK#J.KbKbK.S#F.y.8.8bA#F.l.y.y.8.8.4.4.4.4aPbtaPaP#E.YaPbNaP.Z.sbwbw.C.CbZbB.C.Z#..O.H.OaP#RbX.SbK#JbnaV#a",
"aZ#taZ.Z.4bLbO.ebLbR.S.TbL.T#4buaK.S#R#R#4.Sbw#t#3aT.Z.ZbD#i.E#i#Z#ZafafbN#Z.na2.3ay.B.Bay.B.8a8bn.y.4.4.4.8.8bAbl.y.ybKbS.e.e.8#F.K#s#s.k.KbLbObL#t.Z.9.G.GbI.E.G.9aP.g.ZbZbZ#tbJaI#.#EbW#E.g.Z",
"aKaKaH.u.C.C.sbw.O.O.O.O.Y.Uba.O.uaPbtaT.u.s.s#Rb.#4#5bLbL#T.Z.4.4#t.4.4#t.4.y.ebKbAbV.KbAbAbA.K.KbVbAa5#z#z#z.y#z#zbS.8aB#2.M#2.o#F#ubl#F#u#G.wbQbKbKbnbQ#YaL#Y#saY.KbAbYaBaBbY#.bibiaPaI.Tb7bR",
"#h#5#1#X#jaOaaaa#M#paf.Ybaba.n#S.H#.#.aDaqaqaq.HaPbW#x.Z.4.Z.4bR#RbLb.b..k#4#R.4#RaH.u.u.u.ja1aK#F.S.4.K#zbQ#zbVbSbSbS.ybS#DbVbVayayayblbbbl#u#G#D#u#u#D.1.1#b#H#H#b#b#GbV#s#b.1#L#WaL#WbqaLbK#K",
".zanbeb4be#aaY#aak.k#4bL#t.G.G#i#1#j#j.gaq#j#.bW.ZaT#tbRbObRbLb.#u.r.r.r.r.rbmbm#za5bv.la5a5a5a5.K.4#xbg#xbg.MaM#2.8bV#s.w#sbV#ubS#z.1#z#G#G.1#G.1.1.1.1.1.1bc.1.1.1.1#G.wazaz#Bb4#8bC#Hbebe#H#b",
"#D#G#G#u#0#zbSbSbVbVbAbAbV#z#z#s#JbK.R#s#saYaVaYb.#4.SbL.4.Z#t#t.L.LaS.Lba.O.n#mbQ#z.K#zbQ#0.e#2bQ.K.K.K#z#s#z#D.y#F#F#F#F#F.oaB.8.y.8.y#ubV#0#D#u.r#G.0#G#G#Gbc.1#G#G#G#G#Hbc.1#naz#b.w.1#0#0#P",
".z.zbcbCbcbC.0.0be#b#ba0#H.1.1.w#Dbdbdbn#H#8.z.z#BaL#b.w#sbV#z#s.K.K.K#z.K.K#z.K#R.C.j#RauaubYbt.6#6a6bL#zak#bak#D#D#D.y#u#u#0#0.w#sbV#u#F#Fbl#u.w#0#G#G.1#G.1#G.r#GbV#GbV.1bc#H.w.w.wa0#0bS#G#0",
".w.w.1#G.1#P#G#P.w.1#H.1.1#D#D.1by#0#0#G#P.w.w#b#Hbc#Gay#G.r.1.wbv#F.l#F.laJbvblbA.8.l.8a5#z#zbS.K#4bO.4#TaBaB#Fb4#baz#b#P#zbl.o#F#D#0#G#G#GaU#0.B#u#u#u#G#G#0.1.w.w.w.w#0#G.1.1b4anbraz#8.1#8az",
"b4.1bnbC.1b4.zaz#f#fbr#fb4#8.Van#f#8#8ananbe#G#u.1#G#DbC.1.1.w.1.w.1#G#G#GbS#ubSaiaB.SbXaB#2.4bKbvar.ebF.T#1#j.gah#O##bnbn#D#baY.1#D#DbS#u#D#7#7.o#u#0.wbya0a0#0.wbn#0bSbAbS#z#0#8#8an.z.1bd.1a0",
"#b.1.1bn.1bebran#W.man.manbran.m.m#Bbebe.1#D#D#bbCbCbC#GbC#G.1#G#u.8#ubSbVbS#z#z.w#s#D#ubV.1.w.w#Da7bd#Dbs.5.5#O.Q.Q.K#N.Jbgawax#O.y#z#s#z#z#b#B.zazaz.1#GbC.0bd#7#D#ubSbV#G.wbebr.Vbe#8#D#7#7#G",
"#b#b#b#b.w#b#bbeb4#H.cbebe#bbnbK.e#z#z#sbVbs#D#G#z#z#za5a5#za5a5#sbn#z#z#z.y.8bAbl#F.o.o#u#G#G.r.1bnbn#D#DbV#z.w#T#x.MbGbla5a5.eaB.S.SbT.6.a#2bK.1bn.1bn.1bean#lazazbe.1.R#F.5a#.S.e.ebUbU.W.Wap",
".ybAbAbKbK#DbV.waV#HbebebeaYbnbn#z#FaB#F.y#D#Dbnbe#H.1bn.1#Dbnbn#8#eao#e#8#8#8#8#0#0#0.w#G#G.ray#2.J#2#2.la5#z.w#b.1#u#Gbnbn#D#DbAbA.4.4.8bA#z#z.e.8#T.J#x.l.K#z#D#0.w.w#b.qbraLanbr.Vbr#8bd.b.b",
"#B#B#Bazbrbebebe#8brbrbrbe.zbrbr.mbrb4#H#b#sbVbV#NaW#T#T.J.JbkbkaAbp#Vbpbhbpbpbp.e#zbQ.w.w#s.1.w#H#b.1#G#D#7#7#FbS.8.l.l.8a5#sbybV#G#0.w#0#z#Fa#.4#NbOar#r#r#z#za8.ybS.8.4aB.SbK.cb4anan.mbeanan",
"brbrbebrazbr#f.Vbr#f.manb4bebean.V.V.m.maL.1bA.y.Rbnbnbnbnbn#Hb4#Y.w#saYbQbnbK.RbT.GaM#2.4bVbVbK.8#F#u#z.wbybyby#G#G#zbV#u#F#F.obb#uay#0#0.w.w.w#b#b#sbn#D.X.5.b#z#0#za5bAbA.K#s#TaAbObO#T#T#raN",
"bnbnbnbeanbq#l#Lan.maLbrbebebrbeaVb4b4aVbnbn.wbe#T.4.ybAbK.KbQbQ#D.R#D.1.1#H.1#H#Y#s#zbVbV#z#u.ya5a5ar.l#T.J#vawa7#Hb4be#b.1.0.0#G#ublbv#c#c#2#u#Dbd#D.1.w#Hbean#b.w.wbVbSbVbVbVbVbV#D.1bV.1#ban",
"#Bbebe#abebeaVbnbn#sbn#D#s#a#b.w.q#b.q#Y#Ybe#HbnaY#baY#baVbnaVaV#bbn.w#bbebeaV#H#D.1bebe.w#G.wa0.z.z#HaV#HaYbnaV.ebLa6a6a6bOa5b2.w#b.w#zbU#s#s#zbA.lbv#T#T.l.y#0#8.1#D.1.wazaza0.w#P.wbebe#H#baz",
"az.1bcbc.1.1bCa0#H#8b4b4#H#Hbebr#b#bbn.ybXbK#s.c.ia7bebe#zbV.8a5bnaY.caLaLbe#H#H.kbKbKbK#s#b#b#H#bbV#DbV.wbe.w.1bCbCbn#D#2bg#i#i.J.lbA#s#b#bbQbQ#JbVbVbS#G#u.R.R.SaB#2#2aB#Fbd#H.z.1.wa0#b#Ybe#Y",
"au.S.S#J.k.R.kaY#DbVbV#sbn#s#s#b#Y#Y#Y#Yak.q#Y.N#Y#Y#a.R.y.y.ybV#ebrbq#WaL#B#bbV.i.RbVbn.1#bb4b4#s#D.y#ubV.1#b#H.1.1#bbe#Y#b#s#z#z#z#D#7bs#u.y.lai.5#F#u#0.wbebe.qakbQbQ#baY#b#b.1.1.0#G#D#ubb#F",
".k#s.k.k.K.KbK.KbA.KbK.K#z#s#z.K.5aBbX.4bY#RaHbY.Z.4.e#s#sbnb4#H.q#Y#sai.dbs#8anbeanan#Ybe#H#H#8#b#H.wbV#G#G.0.1.wbV#u#F#7#D.1b4#bbebebeazb4.1bVbebe.w#z#u#FaBai.4#2.4.ebQbQ.w#Y.w#G#GbVay#u#z.w",
"bm#G#Gaybl#ublblbAbA.8bSbAa5bS#F#D.1#zbnbVbK.K.K.K.e.4aB.y#Dbnbea7#8brbq#Y.K.ebU#Hbe#Yanazbeazanazaz#n#n.w.1.1.1.w.w.1#Pa0#b.1.0bV#G.1.1az#b.w#s#sbVbV#ubVbV.1#s#z.yaB#2bG.M#xbgbG.8#z#Yb0#b#s#z",
"bS#GaybS#G.w#b#b#zbV#0bV.w.w#G#G#G#G.1#G#zbV#D#u.y#D#z#s#s.K.8aiar.4.4bV#sbn.cbr#s#u#2.M#x.y#D#b.1#b#b#b#na0a0a0a0.1.0.X#7#u.1a0#z#D#u#F#u#ubVbV#n#n#b#Pby#b#Y#n#bbn#D#DbV.KbV#z.K#1#i.6.6bDbL.Q",
"#Ebi#p.U.Y#Z#C#3bAbAbKbV#s#G#s#0#D#G.w.w#0bc#Gay.1#D#7#F.ya5#s.WaV.cbebn#sbQ#h.T.K.kbQb0aE#YbV.X.y.y.y.ybV.w#b.1#8.1azazaza0a0.wa0.w#0#GbV#z#z#0#F#F#F#F#F#u#ubVbe#b.1aY.wbV.ybAbRbR.Zaq.Z#E.U.U",
".n#Z.p.Las.faQ.fafaf.U.UaPbD.Z#t.4bK#za5#z#G#G#G#G#G.w#G#GbV#u.ybUbQ#z#zbV#JbKbYb.#R.Z#tbA.wbe#n.R#D.y.y.8#Dbn#D#Y.w.wbn.w.waza0.1.1#H#b#P.w#GbV.w#0bV#u#u#u#u#ubv.o#F.y#DbK#z.K#z#4.4.S.TbL#t#t",
"bLaZbD#R#taP.U#Z.hb6as.L#k.pba#ZbD.s.ZbTbG.8bS#Gay.B#G#0#G#D.0#D#s.y.ybVbV.4.ZbL.s.s#R#4#s.w#H.1aY.qbebQ#s#s#sbnar.l.l#u.1#HbC.FbC.1bC.1.1.1.1.1aybVbV.w#s.w#s#0by#za5#u.y.4.l#r.y.8#F.8.K.e.4#T",
"#4#4.S.4#taZ#T#2aZaTbNaQ#daQ.fasaS#QaT.s.4.8ararbl#ubVbV#ubl#F.XbT.G#t.S#tbkbT.T.uaH#OaB#D.1.zaz.z#f.z.zb4.za0.z#Jbn#JbK.ybAbK#s#z#baz.z.z.zaobc.0.1#G.0#G#u#G#G#ubS#0bVa5bS#uaBaB.y.y#u#F#FbA.K",
"#zbK.e.S.8.S.S.T.O#Z.LaQaQaQb5af#..Z#t.4#F.yblblbl#ubVbVbK.S#t.saI#tbwbZaI#t#t.S.n#1.TbF#zbQby.wbebranbraz#b.1.wan#nbe.w#z.y#FaB.8.ybn#Haobcbcbc.1.1.1.1#G#G#G#0#G#0#GbV#ubVbVbV.8bAblbAbSbS.K.K",
"#F.l.8.4bLbRbRbR#Z.L#kafafbN#..Zbg.Jbv.8#u#D.0#7#ubS.y#t.s.Obaas#3baaP.Z.sbD.Z.Z.y#JaY#a#Ybe#b.wb4#b#bbebebebe#bbm.0#u#G.w.w#0bVbGbAbVbV.1.1.w#P.1#Ha0bc.1#Day#D#0.1bS#u#ubV#zbVar.8.l.8a5a5.e#N",
"#T.4.4bObw#.bNad#k.pba#.bR.4bAa5ara5#zbVbV#u.y.4.K#t.gbi#pb5.p#k.ZaP.Z#t.S.y#Dbna7bCb4.zazb4ao.1#b.1.1bVaybV#G.wa0.w#G#z#0#z.K.y.8bV.w.1.1.1bc.1bcbc#Ha0.1.0#G#G#G#G#G#G#GbVbV#ubla8a8blbSa5.8.4",
".e.T#1bw#..paQa.#E#v.Z.8.ebSbSay#FblbA.y.S.T.TaIaDbN#paj.U#v.g#ta6#TbAbSbS#G.1bc#GaU#P.w#0#GbSbS#G#Gay.r#D#G.w#P#D.y.8.4.4.e.e.e#u.1#P.1bc.1.1.1.0#Ha0#Hbc.1.1.way#0.w#0bV#GbV.ybl#F#u#zbVbKbA.y",
".g#.bW.UaOa9#..O.4.8bAbSbS#u.lbv.K.K.S#x#Eb#.v...Yba#..Z#T.4#Fbvbl#ubS#G.0#GbCbc#Pbc#P#P#G.w.w#P#Gbc.w.w.w.w#P.wb0.qbQ.K#r.S.S.S#G.1b4#P#H.1bc.1.1bca0.1#P.w.w.w#G#z#GbVbVbSbSbS#DbV#s.K#4.S.4.4",
"#EaC#i.gbT.4.4bAbAbAbSbAbA.4.ZaPa3.6#6#E#x.4bLbL.4bAbKbAbSbS#ubS#0#P#P.w.w.1.w.1#H.1a7bCbn#H#H#Hbybyby#z#z.8.4.4aM#xaZ#2.8.y#GbV#Gbc.w.1#G.w.1.wbcbC#G#G.1#G#GaybSbSbS.8bla5bA.8#z#z.KbZbRaT.ZaP",
".4#r.K#z#z#G#ubb.8.y.y.y#R.O#Z.tbMbM#5.ebA.R#D#Day.rayaybV#G.wby.1#G.w#G.w.wbV#u.TbObO.Z.Za6bD#ia6bTbDbD#iaP.s#t.K#s.wbya0#P.1bm#G#0#0.BbSayb2#0.1#G#u#GbVay.8.8blbSa5bSa5#za5bAar.Z.gbaa9baba.O",
"b2bS.8#N#T.Ja6bk.M.M.lbS#z#G#z.w#ubV#GbV.1#G#D.ra7bnaV#H#Hbnbn#D.l.4#2.E.6.6bT.T.X.X#u#D.1.1b4be.1#G#G#G.0#Gbcbc.1.1.w.1.w.1#D#G#D.0.0#D#GbVaybS#G.1.1#Dbn#DbnbV.SbA.KbVbK.4aZbD#F#ubVbSaybV.wbn",
"#D.0#Dbn.1#sbQ#s#za5#z.w#zbS.y#ubV.kbVbVbVbVbVbnaW.l.l.4#Narar.8bCbn#HaVbnaVbebea0.w.w.1.1a7#u#7.1#s#H.1.w#Ha0#b#G.w.w.w.1.w#G#G.0#G.1#0.w#0#0.wbS.ebS.e.l.4.JbG.y#u#D.w.w.w#sbQbV#s#0ay#ubV#D#D",
".0bn#Gbn#D.R.R.RbV#G#z#0bV#u#u.y.SaBbYaB#t.4bA.ea5#zb2bSbSbVbV.w.0.0a7#Hb4#Hbn#7a5#z#z.w.wa0a0#bbeaz#b#b#bbn#D.y.1#G#G#G#G#u#G#u.1.1#b.1aybl#Fa8bG#T.8.eby.Wb0bP#G#Gbm#Gbm#G#G#G#sbn.w#z#G#G#zbn",
"#z.w.w#zar.4#T.Jbv#F.ybSbV#ubSbVaY#s#s.Ka5.K#z.Kbl#u#D#G#D.rbd.ra5#0bS.BbVa5#ua8.ybla8#u#0#G#G#G.0#D#G.wbe#B#Ybe#P.w.w.w.1#0#G#G#G.1#P.w.w#0#0.w#baY#s.wbn.0#D#7ay.rbm.r#G#G#G#GbV#0#z#0#G#z.1.w",
".8ara5.Ka5.ear.e.w.w.wa0#bby.w#b#0#0bV#u#F#FbGbv.8.8ay#z#0#0#0#0#z#0a5aya5ayb2#0bV#GbV#D#u#u#D#Ga0a0bc.1bc#G#D#u#Day#D#GbV#G.w#s.w.w.w.w.w#z.w.waBbs.y#D#DbC.1bCbV#G#0.1#0#G#0#zbKbSbSbS#u#ubVbV",
"#Hb4bebeazbeaz#Y#ubV#u#z#u.y.8#F.w.w.w.w#0.w#0#z#G#ubl#F#u#u#u#Fbd#u#D.0#7#7#7#D#u#D#GbV#G.w#bbebc.1#G#G.1.w#P.w#b.wby#s.w.w#s.w#z#z.KbA.4#F#T#2ar.e.ebA.ebAbAbSbQ.w#s.w#z#zbK.4.y.8bSbS#0bSbVbn",
"bn.1a7bnbn#D###u.1#0bn.w#G#z#D#0#0bV.w.w.w#bbe#bb4b4#HbCa7#Dbd.ybs.R#D#u#u#F#7bs#u#u#u.ybA.S.4bLap#b.WbQ#z#s#z#z.8.4.8#2#2bG.MbT.4.SbA.S.4.S.K.Kararar.ea5.ebS.e.8.8.ybS#z#z#s.KbAbla5#0#0.w#s.w",
"#s#z#s#z#z#z#za5#D#D#GbV.wbV#z#0.Ka5bKbAau.R.S.R.lbAa5.e#r#NaAaAa6.ZaW#N#N.e.e.e.BbSa5.earbOa6#Ebi#EbDbDa6#t.4.4.ebA.SbA.e.K.K#zbL.T.KbL#t#TbL.Kbn.1.1.1.w#Hbeb4bV#z#z#G.w#z#z.K#za5#zby#0bVbV#z",
"#u#D#DbV#u#ua8#ubXau.y.y.ybs#DbsbK.SbL#RaIaI.C#3bw#t#R#R.S.S.S.y#t.gaPaP.gaT.Z.ObabaaP.Z.Z.Z.4.l#F#2#2#O.S.ebA.8.S#2aB.y#zbKbS.y#u#D.1.w.1bnbVbn#zbK.y.SbK#4.S#tbSbV#u.8.8#u.8#ubV.y.y.y#ubA#ubV",
".##q#qat.tas.2.2#A#A#AbH.H#C.H#CaMbI#x.GaZ.9bD.G.9bD.GbT#2#2aBaBbK.ybAbKbK.S.S.S#t.Z.Z.4bAbK#zbV#sbV.K.K.K#s.KbV.K.y.SbA.KbK.y.S.4.ybAbK.K.K.K.K.K.K.e.K#4.K#4bLa2#5.D#5a2aIaIa2aIbBbZbwbw.s.OaP",
"ba.O.Obaba#.#.#..A.A#v.Abkbkbk.A.y.8.SbA.e.S.S.4.K.e.e.e.e#z#z#0bS#ubV#zbV.y.ybS#4.SbY.S.y.yaBaB.4#t#t#t#t.S.S.S.e.4.4.S.K.KbL.S#hbF.TbR.gaPbWaP#Z#ZbaaS.p#kb5b5bNa9#..O.O.Oaq.ObaaP.O.O#Zba.Y.Y",
"#D#u#u#uaybSbS#u#s#s#z.w#s#s.1bnbQbQbQbQbQ.Q.K.T#t.4.Z#2#T.4.4.4bAbA.SbA.S.S.K.KbQ.K.K.K#5bLbL.SbO#R.T#5b.#5.K.kb.#5bLbZbZbR.Z.gaP#.aPbNbNbN.O.Hbwbwbwbw#3.O.O.O#2.4.8.8.8.8.ybAbA.e#z#s#z#s#z#s",
"#5.TbLbL.TbLbLbL#m#gae.CaX.Caeae.Z.4#tbLbLbZbR#tbObR#tbR.4.4.4#t.saP.U#E.YaP.U#..UbWbibWbNbNa9#..gbW.Z.sbwaP.G.9aP#E.U#E#EaPaP.U.Y#3bBaIbw.Cb.#w.ybSa5bVbSbV#z.w#z#z#z.K.K.e.ea5bAa5.ea5.ea5a5.e",
".U.Y.YaTaT.Z.O.gbRbRbwbw#tbB.s.ZbTbT.Z#t.4.4.4a6#t.Z#t#t#t#ta6bDaIbZbB.n#m.n#3#3#X#.aq.HbH#..H#m.TbRaIa2.DaI.T#4bwbRbwbwbZb.bob.#4b.buaK#R.s.9#t#3.O.O.Obw.sbw.sbwbwbwbw#tbwbRbB.Z.gaP.gaP.gaP.g",
"aB#FbSbVbV#D#ubVaya8bl.8aybS#u#u#zbVbV#zbV#zbK#u#u#u.y.8.ybA.ybA.4bY.4.4#t#tbDaZ#tbR.4#t.4#t#tbL.K#tbwaI#5aIbLbFaIaIbwbD.U#E.Ubi#EbDbD.ZbT#xbT#xabaRaR#oabat.O#Z.s#t.SbLaI#4aKb.b.b..Kbu.7.Qbub.",
".w#b#b#Y.w#0.1#G#z#G#z#G#b#b#b#s.1#G#G#G.1bc#G#G#G#Day#G#G#G#G#G.w#G#GbV.1#z.w#b.kbnbKbn#s#DbVbV.K#t#iaP.ZbW.g.Z.s.s.gaPaP.Zbw.Za6.Z.Z.8#r#z#ra5#D.r.r.r#Gbcbmbm#G.1.w.wbc.w#G#G#G#G#G#Ga0#P.1.r"
};



/* XPM */
static const char *button_xpm[] = {
/* width height num_colors chars_per_pixel */
"    96    96      254            2",
/* colors */
".. c #9c3218",
".# c #a4733e",
".a c #bc450a",
".b c #d4700c",
".c c #dc8c29",
".d c #bc5e00",
".e c #d46b37",
".f c #945431",
".g c #bc5a2c",
".h c #8c4e4b",
".i c #d47e16",
".j c #bc7422",
".k c #d47d3a",
".l c #d45e28",
".m c #ec9b3e",
".n c #bc6b43",
".o c #d45a13",
".p c #a45236",
".q c #dc8848",
".r c #e46b1b",
".s c #bc652f",
".t c #a46243",
".u c #bc6920",
".v c #a4391e",
".w c #e47b35",
".x c #9c6b30",
".y c #d46d24",
".z c #ec8a29",
".A c #bc452a",
".B c #e46833",
".C c #bc702e",
".D c #c47845",
".E c #b15314",
".F c #e47e10",
".G c #bc5a18",
".H c #bc6145",
".I c #a44d16",
".J c #cc5728",
".K c #d47439",
".L c #a45b39",
".M c #cc5815",
".N c #dc8f4c",
".O c #b46239",
".P c #bc5307",
".Q c #d4794f",
".R c #d47624",
".S c #cc6c2c",
".T c #cc6941",
".U c #ac5222",
".V c #e49328",
".W c #e4754d",
".X c #d4650b",
".Y c #ac592c",
".Z c #c05e2d",
".0 c #e4751a",
".1 c #e47d29",
".2 c #9c6143",
".3 c #e45f2b",
".4 c #cc632a",
".5 c #cc660d",
".6 c #b44b13",
".7 c #d48442",
".8 c #d66228",
".9 c #bc6221",
"#. c #b45736",
"## c #d47714",
"#a c #dc8936",
"#b c #e48237",
"#c c #cc530c",
"#d c #8c522b",
"#e c #dc8819",
"#f c #ec9129",
"#g c #bc7936",
"#h c #d46f50",
"#i c #bc521d",
"#j c #bc553b",
"#k c #9c523b",
"#l c #f49a45",
"#m c #bc7548",
"#n c #ec8643",
"#o c #a86d3a",
"#p c #a44d21",
"#q c #a4754b",
"#r c #d46547",
"#s c #dc7937",
"#t c #c4642d",
"#u c #dc6c21",
"#v c #bc4d2e",
"#w c #cc7e53",
"#x c #c45917",
"#y c #cc5c42",
"#z c #dc7036",
"#A c #ac5448",
"#B c #ec8f44",
"#C c #b45c4d",
"#D c #dc7622",
"#E c #b45222",
"#F c #d4651b",
"#G c #e47328",
"#H c #e58429",
"#I c #9c5f23",
"#J c #d47f2b",
"#K c #cc7116",
"#L c #eca24e",
"#M c #9c471c",
"#N c #d45b36",
"#O c #c46716",
"#P c #ec7836",
"#Q c #ac642d",
"#R c #c46f2e",
"#S c #c47551",
"#T c #cc5b2b",
"#U c #cc685f",
"#V c #bc4b3a",
"#W c #e49a4b",
"#X c #b44c35",
"#Y c #e48745",
"#Z c #ac613c",
"#0 c #e47234",
"#1 c #c46242",
"#2 c #cc5e1a",
"#3 c #b4683d",
"#4 c #cc7430",
"#5 c #cc7042",
"#6 c #bf4b0d",
"#7 c #dc6e13",
"#8 c #e48c21",
"#9 c #9c3445",
"a. c #8c432b",
"a# c #cc5e04",
"aa c #a43f33",
"ab c #b46d44",
"ac c #9c5e62",
"ad c #9c4833",
"ae c #b4742c",
"af c #a4522b",
"ag c #dc943c",
"ah c #b46000",
"ai c #c46309",
"aj c #a4441e",
"ak c #dc7f4c",
"al c #ac6e27",
"am c #ac4b4e",
"an c #ec9238",
"ao c #e48518",
"ap c #e47c4f",
"aq c #bc5c3e",
"ar c #d56238",
"as c #9c5840",
"at c #ab6946",
"au c #cc761f",
"av c #9c5024",
"aw c #bc4922",
"ax c #bc421d",
"ay c #e46927",
"az c #ec8836",
"aA c #cc513b",
"aB c #cc661d",
"aC c #b44a21",
"aD c #b4544c",
"aE c #ec8a55",
"aF c #9c5a20",
"aG c #94533f",
"aH c #c4701d",
"aI c #c46b42",
"aJ c #dc5a18",
"aK c #cc7b2f",
"aL c #e49046",
"aM c #c4520b",
"aN c #dc774c",
"aO c #ac4631",
"aP c #b45b28",
"aQ c #944e30",
"aR c #ac6e4d",
"aS c #a45f2a",
"aT c #b4612a",
"aU c #ec7032",
"aV c #dc872b",
"aW c #cc5939",
"aX c #b47844",
"aY c #dc843b",
"aZ c #c4601d",
"a0 c #ec7f36",
"a1 c #c47531",
"a2 c #c47344",
"a3 c #ac431d",
"a4 c #ac7439",
"a5 c #dc6735",
"a6 c #c4582a",
"a7 c #dc7c1b",
"a8 c #dc5d26",
"a9 c #ac5036",
"b. c #cc7742",
"b# c #ac4b13",
"ba c #ac5a37",
"bb c #dc5f0c",
"bc c #ec7a29",
"bd c #dc7813",
"be c #e48b37",
"bf c #f48e28",
"bg c #c44e1f",
"bh c #c44e3f",
"bi c #ac4b1e",
"bj c #dc6544",
"bk c #c44c2b",
"bl c #dc611a",
"bm c #ec7125",
"bn c #dc7d29",
"bo c #cc7752",
"bp c #c4473c",
"bq c #ec994b",
"br c #e49336",
"bs c #d46f16",
"bt c #bc600a",
"bu c #d47a46",
"bv c #d45b1e",
"bw c #bc653a",
"bx c #ac3c1d",
"by c #e47943",
"bz c #a46b2e",
"bA c #d46b2e",
"bB c #bc6c3a",
"bC c #e47f1b",
"bD c #bc5b22",
"bE c #bc6052",
"bF c #d47346",
"bG c #cc561f",
"bH c #b46044",
"bI c #bc5312",
"bJ c #d4775e",
"bK c #d4732e",
"bL c #cc6b37",
"bM c #cc644f",
"bN c #ac512d",
"bO c #cc6137",
"bP c #ec8552",
"bQ c #dc7944",
"bR c #c46339",
"bS c #dc6a2c",
"bT c #c45a21",
"bU c #dc6f44",
"bV c #dc732c",
"bW c #b4502c",
"bX c #cc6d1f",
"bY c #c46521",
"bZ c #c46939",
"b0 c #e48653",
"b1 c #ac6445",
"b2 c #e46e43",
"b3 c #c45851",
"b4 c #e48b2b",
"b5 c #9c5430",
"b6 c #944d49",
"b7 c #c46a51",
/* pixels */
".waB.U#5#Dba.##u#sbn#H.8#z.0#Db2.4#E.g.e#T#F#z#4bL.n#EbSbm.kauaz#Bbnbr#B.y#b#bb4.w.z#D.z#haKaZbr#Ha6bLaubn.w#Yb4.z#0#ba7an#s#Yb4b4.8.wbnaVaOb3aBbS.l.K.4bL.S#i#5#0#u.w.w#u.w.1#D#zaP#AbK.y#.#a#u",
"#b#F.Y.T#u.O#q#D#z.1b4ar.wbn.0bS#raC#..T.4.lbK#4aZ#Zbi#G#G#s.S.1bebnbr#BbA#b.1.1.w.z#Gan#5aK#tbr.1a6#1aua7bS.w#HbCa5#0#DazbK.w#8#H.8#0aVbXbibRbsbA.l.K.e.e.4#x.T#0.8.wbVbVbn#sbVbSaT#AbVbS.gag#G",
"#bbS.YbL#u.O#q#D#sa7bea5.w#G#D.8.K#ibW#1.4.8.e.SbD.p#pay#G.k.Sbcbebnbe#BbA#b.1bn.1bc#Gbe#1aHaZb4#D.Zb.aLan#0#Dbda7bS.ybCbe.y.w#8be#ubSaY.G#X#1.ybA.8#z.K.ebL.ZbL#0#u.w#GbV.w.1bnbA.u#C#z.y#.aL#G",
"#YbVaTbL#ubaatbV#zbnbe.K#zbnbn#N#z.g.UbwbO.4.S.4#R.L.UbSay.k#Jbc#abebrazbK#bbnbC#GbC#ub4#X.u.Z#HaY.eaI#Wbr.w.1#H#HbV.8b4anbS#s#H#b#GbVbe.6bw.4.ya5.y.ebS.e.e#t#t#G#u#zbVbV.w#s.w.e.u#CbKbS#.br#P",
".wbVaT.Tayba.t#u#zbnaza5ar#D.1#T#zbTaO#.bwbL.8#t#tas.Y#Gbl.K.k.1beanazbrbK.w.1.1.1bc#0be#j.C.4b4.ybO.g#Wana0aya7az.w#2az#B#z.w#HaY.wbVaNa3.s.4bA.e.8#F.8#z.ebLbD#0#u.w#GbV#b.1#s#z.s#AbAbV#jag#G",
"#0#D.ZbLbS#.as#u#z#Dbe.e.4.R#s.J#G.4a9.p#.bR.SaZaP.f#Z.w#u.K.R.1bebqbrbe#D#bbeb4#PbC#z#aaO.CbL.1bV.T.Ibr#8a0.w#Han#Y#2anan#z#0bC#s#H#DaraC.s.4.lbAbA.lblbS.e.K#i#ubV.w#s#GbQbV.wa5.CaD.ybV#.#a#G",
".1#u.ObLbS#..2a8#z##azar#T.RbQa6#u.4#.aQbNbR.S#T.UaQ#C#bblbK.kbCaV#l#fbebV#bbr.z#G.0bSaYaa.sbO#D.S.Q.U#Laz#n.w.1b4#B#2anbebS#zbC.KaV#Dbpb3bYbX.l.8bAbvblbS.e.k.g#u#G.w#GbVbVbS#s.e.C#CbKbV#X#a#G",
"#GbV.gbL#u#..2#ua5#u#Y.e.J.R#sbkbbbA.Oa.adbR.T#2#Z.f#3#bbl.KaYa0bn#L.Vbe.wbeanaz#P.0bS#aaabw.e.0aBaNaP#W#8a0#G.0#8aE#2an#HbS#0.1.S.1.1.A#U.S.Rar.4a5#FbSblbAbQ.ZblbV.w#0#DbV.ybV.8.CbE#zbVbW#a.1",
"#zaybR#m#s.A#AbX#D.1#u.wbvbV#z.M.8bA.4#E#k#Z.OaZ.hafbA#zbAbA#D#Hbnanbr#8aVb4#W#f.wbebVak#M.ObL.R.Ya2.Gbe.1aza7a7#Y.1#F#B.y.k#Db4.y#saY.P.ybX.K.4.8bS#zbS#u.8bA.SbDby#G.1#s.4.KbAbV.SatbVbl#Z#D#G",
"#Ga8bR#g#s.A#Aau#D#0bV.w#F#Ga5.M.ybA.8#v.p.L#ZaTb6afbAbVbA.KbV#8#s.m#fbr#H#H.m#f.1#bbV.k#p.ObR.y.Oa2aZ#n#baz#H#H.w#0#uaL#O.k#D#b#F.w#b#2#zbK.KaB.ybS#GbS#DbSbVbAbT.wbV#G#s.4#zbS.KbAat#z#u#obV#G",
"#zblbwae#z#v#A.y#Gbn#u.w.y#z#z.l.ybSbA.Zba#k.LbNas.UbK#0.8bKbVb4bnaL.mbrbe.canbr#H#bbA#4af.O.S.y.OaI#tbea0#b#H#b.y.w#G#b#x.KbV.w.y#s.w#F#s#zbVaBbS#ubS#GbSbSbK#z.4bQ.r.1bn.y#sbSbV.SaR#0#u.Obn.1",
"#G.8bw.C.w.AbH.ybV.w#za0bS#0.wbS.ybAbS.8#.afaQaQ.L.UbVbVbS.K#sb4#Dbranbrbebe.m#f.1a0bAbL.Y.O.T#F#.aT.4.w#b#H.w.w.8bS.w#Y#x.SbV.1bS#0#z.y#0#z.y.8#zbS#ubV#GbS#ubA#4.w.0.1#bbKbVbV.y.4b1bS#u.O#G.1",
"#bay#taX#sbk.H.y.w#G#u#bbVbV#z#z#RbAbS.ebRafaQ#d#kaP#s.wbA#zbn#H#sbeb4bebebeanb4.1#HbV#tba.YbL.y#X.U.S.wbc.1#GbV#zay#0by.y.SbV#PbS#zbS#u#z.wbl.y#GbS#u#G#GbV.ybK#z#b.0.1.wbnbV#DbA.Sb1#0bS#3#0.1",
"#bbSbB.C#sbk#CbsbV#z.yby#u#ubS#G.O.4#ubS.4bNaQaQ.pbD#G.wa5#s#s#H#abebe.zaY#bbr#8#D.1#z.Gba.U.TaB.Ob#bK.wbe#G#G#s.w#u#G#bbKbAbV#G.RbV#F#s.w.w#F#F.w#0#G#0#0aybSbSbQ.w#G#G.wbVbA#GbKbAab#0ay.n.1.w",
"#b#u.sae.1bk.H#D#z#D.8.wbS#u.y#z#Z.Z.lbSbA#.b5.fba.Z#s#GbS#z#sbe#bbrbebrbnbnan.V#D.1#z.G.nba#4bvbB.6#s.1az.0#G.w#Gay#P#GbKbK#b#G#z.waB.w.w.w#F#u#0.w.w#G#G#0bVbS.ebV#G#G.1#z.8bS#z.8b1#0bS.n#0#G",
"#s#u.Zaebn.A#Cbs#0#0#F#bbV.y#u.w.taPbvaya5.Zafas#Z#t#0#G#F.K#bbr.wbeanbrbnbK.man.1.w#s#i#S.ObuaBaI.EbQa7a0.0#G#0#D#Gaz#u.Rbnan.0bebQaB#s#sa0#F.wbS#G#G#G#G#G#zbA.ebl#G.0bV#z#F#0bK.8.tayay.H#D.r",
".1#zbT.ZbQ.yaMbK.K#0.w#0aY.SbV#ubMa3.K#Farbg#.aSbD.4#D#G#D.5#Y#b.qaV.V.m#z.e.m#fby#D#J#1.H.uaK.ZbRbIbe.1a0.w#D#D#Gbdb4b0.J.wa7ak#H#n#xb4#H#z#zbnay#G#u#G.1.1#G#u#s#Tbd#0b2#GbA.1.8#r#Ia5#sala5#D",
"#GbVbT.4bQ.8bI.Sa5bV.w#0#saB.kbVbM.6.Kbla5.J.Z#Q.sbK#G#G.1aB#Y#b#bb4.Vbr#F#z#B#8#0bdbK#j#.aP.S.g.TaZ#B#H.w#P.w#D#0.Fb4aNbv.1#DbV#Ha0.Mb4#H#za5#D#0#G#G.0.1.1.w#D#s#Ta7#0b2#D.8#H.y#r.fbA#zal#z.1",
"#GbV.Z#tbQ.S#xbLbK.w.wbV#sbYbV#G#5#6.SbA#zbv#taT.Z#z.w.1#zbX#Ybn.qb4.mb4aB#zbe#8#0bd.R#j#.bt#R.g#1aZbe.1#G.wbV#D#PbCbe.ear#D.1bX.1by.l.1.1a5a5#Dbm#G#G.1.1a0.w#G#s.la7.wa5#Dar#HbVbF#I.4a5ala5.1",
"#G#z#tbLbQbA.G#RbA.w.w#u.KaBbVbV.e#E#x.ybV.8.4.sbTa5.w#Gbn.4#Y.y#YaV.m#H#F#sbean#Gbn#s.gaDaT#R.g#t.4#b.0#G#G#G#D#0bCb4#x.e#G.1aB#G#0.8.1.1bS.B#G#G#G#G.1.1.1#G#u#z.e#H.w#z#D.8#H#z#haS.4#zal.4.1",
".1bV.4bLbQ.eaZaIau.w#0#Fa5#tbV.1bA#x#E.SbV#u#F.4bG#z#0#zbVbYakbX#YbnaL#b.ybV.1an#P#H#saqaq.u#4#j#1.S.1#u.1#0.1#G#0#Hbn#ibjbV#b#2bca5a5.1.0.l#z.0#G.1.1bcbc#b#D#u.war#8by#z#u.e#b.y.Tbz.4#za4.J#D",
"bc#z.4bZ.Q.S.9aI.R#b.w#F.K.4bV#G.R.4b#.T#u#D.y.8.8#GbcbVbK#R.qbKbebn.1#s#Dbs#Dbe.w#8aY#jaq.s.SaO.Z.S#D#7bm#G.w#u#0.zbn#xa5#G.1#2#bbSby.1#G.l#0.0.0bca0az#Ha0#G#u#0.B#8.w.K#D#r.1#FbO.x#T#zaX.J#D",
"#GbK.4bR.K.SbD.C.Sbe#0bG#zbAbV#D#DbL.v.T.y.0blarbS#G#G#D.KaH#Y#s#H.wbAbV#D#D#D#G.w.zaV#.aq.sbwax.gbA#D#7#G#u#G#u#uan##aW#NbV#D.4#bbvap#H#H.8#sa7.1.1bc#Ha0#H#0#D#0a5#8bQ#z.y.ebn.y.T.##x.KaXbkbV",
"#G#ua6#t.T.4.G#3.R#b#zbv.K.ebn.r#DbL..aI.4#7blar#G#Gay#u.KbY.N.cbnbe.ybVbn#G#b#u#b.zaYbW.H#R#tbx.Z.k.w.r#0ay.1#GaJaz.5bO#T.w#DbAbnbg#b#Hb4bVby.F.0bC.1.1.1#b#GbVaU.8#HbnbA.R#N.1#s#h.#bgbAa4#vbV",
"#G#u#tbO#t.K.9bw.lb4#G.8bla5aWa7ay.4.YaD.K#ublblay#G.1.y.K.Z#Y.iaY#T.R#Nbe#zbC.1#H#Bb..ZaPb.#3aFaPak#G.0.1#D#D#G#D#b#xbAbv#G#ubA.w.Mbn#nbVbS.1bdbcbm#P#G#Gbc.1#G#D.w#u.w.w#2bK#4bK.K#vbw.Q#tbt.K",
"#D#u.ZbR.4.ebD#tbAb4#u.8#u#z.lbn.rbAbabN#tbS#u#u.B#G#D#D.e.4#Ya7#b.4bnaW#H#zbC#GbcaL#4aTbW#4aTaS.UbQ#D#u#z#D#D#G#u#baBa5bS#G#u.K#b#F.1bebVbV#GbC#H.1#P.1.1#H#GbVbS#G#u.w#saBbA.SbAbF.gaqb..u.G.K",
"ay.y#t#t.Z.e.G#Ra5#Hblay#Db2.laVaybK#.#p.g.ybVbV#G.w#7#z.4.e#abeaY.ybn#T.1#zbC#D#G#b.S#t#x#5.Z.Y.U.kbSbl#GbA#u#G#u#b.4.e#z#G#FbAby.ybeaz#ubS.1bc.z.1#P.w.1bc#DaybV#G#F#G.w#2.S#t.R.e.g.OaI.9#x.K",
"#G.8#tbR#2.ebT#R.ebC#F#z#GbS.4#HaybA.Zajbi#tbVbV#0#G#F#saB#s.Rbe#bbAbn#Tbna5#GbCay.wbLbR.ZbL.Z.O#E.Kbl#ubVbKbSbVbl.w.4.8#0#ua#bVbQ#ube#b#u#G.1.1bf.0.w#Pa0#H.1#GbV#G#u.w#s.M.S#tbn.e#1.gbZ#R.9.K",
"#G.y#t.4#T.e#2.S#ra7#u#0#DbS#N#HbVbS#T.U#p.sbK#u#G#G.y#s.y#s.y#zaVbKbn.J.1a5bC.1#G#s.4bO.4bLbD.sbWbF.8ay#z#zbA#u#D#z.4.4#0#u.o#s.4bSbe#H#u#G.1.1.z#D#G.w#ba0.1#0#D.w#D.wbV#x.S.S#s.SbR.ObZ#5bYbO",
"#GbA#t.4.4#z#2.S#N#D#u#0.rbVarbn#GbS.4#vb5.O.Sbl#DbVa5.K#Dbn.ybVbn.Kbn.J#D#z#G.1.rbV.ZbR.Z#T#i.s.g.ebl#G#0.K#u.8#0bA.e#2.1#u.y#b#xbAbebn#D#Gbm.0beblbV.w.1bc.1#G#D.1#Ga0#s.M.S.Tbn.S.T#jbBbu.4.Z",
"#G.ya6.4.4#zaB.SaAbd#u#0bdbVarbn.w#u#F.g.pba#t#F.0#u#s.8bnb4.y.8aVbQ#Hbkbna5.1.w.1#z#tbL.4.Z.E#t.gbA#uay#sbS.K.y.w#2#z.4#0bl#D#Ybg#zbebC.0.w#G.1.1bvar#z#u.1.1#G#D.1bV#b#z#2bLbL#b.S#5#..g.k#t.Z",
"#GbAbD#t.4#0aB.yaA.y#F#0.r.w.8#DbybSbv#t#kas.s.X#D.y.Waibe#HbVa5aVbQb4bkbna5#G.1.w#s#tb.bR.4#ia2.g.y#F#u#GbKbV#ub2#c.K.4bV#7#DaYa6.kaza7#D#P.1bc#z#Tar.8#F.0.1.r#G.1#u#P#s#2bL.S#b.S#haP.U#5bY#T",
".w.4aI.sbAbSbK#ta6bsbd#za5.0bC.l.1#0bla6.Z#3aIbT#sbUaVara7.q#ebn#b#D#YaA#8#s#u.wbv.K.L#u#R.4#Z#F#T.e.8#z.e.l#sbs.w.P.e.w#G.r.r.1#T.wa7.z.1bea0.F.K#D.RbK.e#zbV#D.w.1#G#u.w#zaB.K#Y#ubOaC.gbobwaP",
"#GbYbZaPbA#u.y.g.Z.R#u#0#0.0bn.4#G#P#u#TaPba#t.G.ybQ.c.4#8#YbraYbn.R.wbp#ebn.8.1#F.K.L.rbL#t#Zbl#T.eaB#z#z.4#G#7.1#6.S.w#u#G.rbV.l.w.0be.1.wa0#8bV.R#J.K.4.8bV#G#G#G#G#G#b#saB.K.w.ybOaCaPb7.s.G",
"#G.4bB.U.SbVbAaPaW#D#Da5bSa7#H#2.w#PbSbA.ZaPbw#t.y#zbe.4br#sbq.c.w#D#s#Vao#z#u#G.l.KaS.rb..4afa5.4bS#Fa5#zbv#u#G#0#6bAbQ#u#G.w.8#z.wbd.z.1#b#H#HbS.y#HbQ#T#T#zbn#G#G.w#G.w.waB.K#s.8.T#v#.aIaPaT",
"bV.4.n#EbA#zbKaP#N#u.0ay.B#HaV.E#G.w#GbS#t.ZbZ.SbV#zbnbVbqai#WaL#b.1aYbp#e#zbS#G#F#z.L.rb..4afbS.lbS#F#u#z.8#u.1#0.M.8#z.o#0#baZa5.w#u.1#D#b#H.1bAbVaYbQ.8.J.y.w.w.w.1#0.w#s#2.SbVbA.K.gaPaIaP.Z",
".1#t#m.Y.SbVbK.g#N#u#7a5bVb4bn.6.w.w.0bS.S.saI#tbVbV#s#s#Y.daLaLbe.1bQbh#8#zbV#G.l.Kba.r.k#tbNay.l#u#F#ua5#F#u.1.w#2bAbS#F#z#Y.GbVa0#u#D#7az#b.y.ybe.1bVby.laB#b#P.1.w#G#GbVaB.S.y.e.K.g#..n.U.O",
"#z#t.naP.S.y.SaT.e#F#7aya5#HaV.6.w.1#G#G.ybD#tbk.4#JbQbn.Kbs#Bbebe#Hbnbp#8.ybSbSaJ.K.O.r#4.4#Z#u.8.8#ubS.8.lbVbn#z.e.ebSbbay#bbY#Ga0bS#D#Dbr#bbG#D#n.1#u#b.eaM#b.w#G#G#GbV#s.SbK.4.4#5aPaPbwb#.O",
".wbD#3.U.K.y.S.Z.e#7#7b2#ubnbebTbV.wbC.1#D.Z#tbT.ZbK#h.c.e#8#b#HaV.1bKbp#8.8#z#ubv#z.nbm#R.y.na8bS#FbV#zbG.ebn#D.4.KbKbV#7ay#baB#G.wbS.w#Hb4#s.4#Daz.1#D#nbV.M#H.w#G.w#G.w.wbK.e.lbA.KaPaPbwaf.O",
"#baZ#3#..KbS.S.O.ebs#D#0a8#7be.T#u.1bcbcbn.Z.S.TbLbY.TbrbUanbV#H#H#H.Rbp#8bA#zbSbl.K#mbm.4.ea2bl.ebG.wa5bg.S#s.y.G.k.KbVbm.0.w.Sbc.w.B#Bb4#H.y.e.1an#H.1a0bV.5aY.w.wa0.w.w#sbA.SbA.ebu.Z.O#m.Y.H",
".k#t#X.UbQ#4#tba.B#u#ubV.ya5a0.X.T#H#P#Ga7.y.n.u.sb..K#s#Hbe.i.k#D#YbT.e#0bl.waibA#RbQ#z#RbK.3.y#F#r.y#DaA#u.z#x#v#K#z.w#F#Y.g.R.w#HbCazbCbea#bnbean#H#0a0#G.y.1#G#PbV#b.X.1bV#t.y.KbL#R.9#5.s.9",
"bnbR#.bW.K.S.ZbabS#u#D#Gbl#z.w.XbO.1bcaUbC#J#1aH.s#R.k#ubean.RbK.1#s.G#z#0#F#saB.8.C#za5aHbAay.y.8#rbS#ubvbAb4bXbhaBbV#z#7.wbkaV#b.w#D.zbcbe.X#sbean#b#G#b#G.ybVbm.w#Daz#D.wbV.4.y#z.4#RaPaI.saP",
"bK.4aqbi.KbY.ZaPa5#u#GbVa8#z.w#ubOa7#P#Pb4aY.T#O#R.ZbQ#2#YanbVbKbe#zaMbQ#0.o#D.S.l.j.Kbv.ubV.B.R#F#rbS#u#TbAb4bAbR.X.y#z#ua5bg#a#b#H.0#H.1be.y#sbebe.1#G.w#G.ybVbm#G#D#Ya0.wbV#t.ybQ.4#t.ga2.Z.Z",
"bn#t.HbW.K.S.4.Z.e.ybV#D#u.w.1#D.ZbC#P.w.z#abFaB#4#tb0.Man#YbnbKbebV#2.w.w.o#ubX.8#R#z.l.u.K.BbK.l.e#u.8.l.R#sbAbObs#ubS#DbS#Tbe.wbV#D.w.w.1.y#b#Haz.1#G.w.w#u.ybm.1bVaza0#b#s#tbA#s#t#t.s#5.saP",
"#s.4bHbN#5.ybA.ZarbA#G#u#0.w.1.1.Zbn#G#0az#Y#z#D#sbAaE#xazbe.1#s.wbV.4.w#G#ubVaBa5aubQa5.ubAay#J.8#F.8.8#FbKbObY#rbnbV.J#D#zarbnbV#D#G#P#b#ubVaYbe#bbc#G.w.wbV#uay.w.w.1#G.w.wbD#T.K.4#t.O#5aPaP",
"#D#t#.bNbL.ybK.ZbO.S.w#u#Ga0a7.1a6#H.w#Gb4bebQ.1.w.w#Y.ybe#H#b#b#G#zbV#s#G#G.1#2#zau#0a5.jbA.B.Kar.y.8.8#D.K.gbtar.1bVbGbnbS#z.1#D#u.w.w#b#Fbn#saz#b.1#G.w.w.w.y#u#P#b.0.1.1#YaP.M.K.Z.Z.ZaI#EaP",
"bV#t.Ha9bLaB#z.4a6.4#b#D#Ga0#ub4bD#H.wbSao#bby.z#HbebV#Daz#Hb4#b.w#ubV.1.r#G.w.4#zbY.ea5a1bA.8bKarbsa5.8a7.K#V.Z#N.1bVbG#b.8a5#8.1#G.w.w#n.5.waYazbc#P.1#P.w#P#F#G.waz#Dbc#Gbeb#bGbU#t.saPb7.gbZ",
"bVbL#m#..SaBbV.l#EbLbe#G#G#b#7be#i#H#PbS.1.w.waz.1#n.X#ban#8b4#Ha0.ybK.way.r.wbKbSbt#2a5aK.Ka8bKarbd#z#ra7#s#X#R.l#7bV.lbe.Ma5azbe.1.1.w#n.X#baVaz#H.1#P#0#P.w#F.w#0.w.0az#GbQb#.4bQbL.Z.Z#5bwbo",
".K.K.T.gbO.4#s#Fbiapbca0.0be.1.1a6by#G#G#bb4be.zaY.R.y.1az#b#s#b.za5.8#H#2.1#Dbv.K.6bQ.K#F.Kbn.SblbV.k.4bQ.4.MbX#4#Har#D#s.4###bby.1anbebe#Fbe#baz.w.w.1bV.w.wbvaz#D#G#G#G#s.K.UbO.y.e#O#C.D.U.i",
"#t#tbRbW#R#tbV#2#E#b.1a0#Daz#s#GbTbybc#G.1#bbr#f.q#D.y#baz#H#DbV.za5#F#b.Jbna7ar#4#6.K.4.SbV.y#FbS#z.KbYbybAaBbKbLbn.8#G#s.Z##.w.w.1azbe.1.ybe.waza0#b.1bV#b.w.o#b#D#G#G#G#zbA#EbO.R#r#OaDbB.U.k",
"#ibwaI.Z.T#t.K#2bD.W#Gbc#G#b#H#GbDby.way.1#ban.zbe.y.y#b#n.w.y#D#Har#u.1#2bnbd.ebOa6.K#x.4bA.4.ya5bV.KaZbQa5.y#s.Sa7arbV#s#x.5.wbV#bb4an#u#saY#sbe.1#bbe.1#n#b.M.wbV#G#GbSbV.e.9.4.y.K.5#AbB.U#J",
"aPaIa2.s#5#t.K#ObDbQ#G.1.w#b.1#GbD#z.w.rbVbebr.zbQ.y.y#b#nbV#ubVaV.l#z#G#2#D#DbF.4bL.Kbg.Ka5.4.8#0.ybK.G#z#zaB.k.4#Da8.1bnbk.b#bbV.wbebr.y#Y#s#JbV#sazaz.1#b.1#2.w.w#G#G#GbV.K.S.S.y.KaHam#3aO.k",
".Z#5.Dbwb.#t.K.Sa6#z.1bcbe#b.w.0#i#z.w#Daybeazb4#s.8bV#n.w#GbV.w#H#T.w#D.l#Dbs.T#T#z#z#x#z#z.4.8bSaB.K.G.e#zaB.R.4#Dar.w.wa6.R#b#z.1bebeaB.NbA.S.SbVbebe#G.1.1#2#G.w#0bVbVbVbV.ebLaB.e#KambwbW#J",
"bWaIaIaP#5.S#s.e#t#s.w#G#Bbn#H#GaP.8.w#GbVbe#b.z#s#D.wa0.1#G.1beaY.Jby#7a5bV.5#1aBak#sbgbQ#z.8bA.BaBbQaZ#z#z#2.S.4#Da8.1#z#Ta7a0#s.1bebnbXaNbRaZ.4.S.w.1#G.1.w#2a5#0.Bbl#ubS.ybVbO#O.ebYambwbWbK",
".gbL.T.G.K.S.KbA.4#z#P#D#Y#Da0bc.s.4#P.w#Gbe.1a0#sbn#ba0.1.0#b.wbn#vby#7#z#z.5#jaB#b#z.M#z#z.8#FbS#2aYaZ.e#zaZ.R.4#Da5#GbVbO.1by.w#Dbr.R.S.QbZbD#1.S#z#G#u.w.waBbl#0bS.8bVbVblbSaq#x.T.Sam.CbWbn",
".ZbF#4.9.k.SbV.8.4#z.w#ube.y#bbc#t.4.w#P.w#b.w.zbn#D.1a0.1.1#H.1aVawby#F.w.w#O.g#Fak#DaMbV.ybA.l#u#2akaZbA#z#2bAbL.0ar#GbVbObn.w#Y#D#BbX.k.Qa2.sbM#tbA#u#G#nbybG#u#0a5bl#0bV.y#G#.bT#r#4#X.OaPbn",
".saIbwaPb..e.K.S.e.8#b#D#P.1#G.1.KaMb0#Da0bman#Jar#Y#8a0.w.w.1bC.ea7#GbS#b#T.Qahb4#D.y#2bS#zbl.ybV#Mbu#tbl#w.yblay#0.R#Hbk#D#H.1#bbV.V.4b4.TaubRbZ#t#t#z.1.1#b.9ar.w#ubv#zbdbs.K.O#t.KbD#3#ob##b",
".saIbR#E#5.4.y#2bA.4.way.w#G.w.1#s#x.q.y.w.0#nbn.l.w.1.1.wbV.1bCbL#H#G.8.1#x.Q#O#b#D#F.8bS#z.y.y#uav.K.Sbl#m.y#F#D#zbV#8bp.RaV#Hbeak.cbQ.c.T.S#tbR.s#tbK#G.1.w#OaW#b#ua8#0#ubs.K.Y.4#z.G#Zabb##s",
".gbwbw.UbL.4.SaB.S.8by#D.w#G.w.w.waZbQ.8#G#ube#J.l.waz.0.1#u#bbna6b4#z.l#u.M.K##az#D#FbVbSbS.y.8#uaS.SbLay#3.R.o#ubV#z#8bpbn#HazaYaL#Db0b4bu.R.S.Z.Z.Z.K#G.1#baB#i#b#D.8#0bdbs#rafbL.K#x.Oab.EbV",
"aPbDbw#EbZ.SbA.ybA#2#s#G.w#G.w.1by#2.K.4#z#G.wbK#ubnaz.X#P#Fbe#Da6bebV.l#GbG#Nbn#b.y#F#s.y.8bK.8bl#Z.4bF.0ba.K.oaybVbV#8bp#s#Hbe#JaL#OaLaY.w#s#z.Z.s.Z.4#u#G.w.y#iaV.1.Ba5.r#D.l#pbL#zaZaP.n#tbV",
"aP.UbZ#EbZ.K.K#z.e#2.wbV.1#G.1.wa0.8#r.4#0.w#z.y.1.waz#7a0#7#Y#2a6#b#u.8bnbl.Jbn#P#u#F.wbSaBbS.4#u.O.4b.ay.pbQ#F.1#ubAaobh#bbnbe#saLai#BbA#s#z#u.Zbw.ZbObS#G#b#D#iaV.1#z#0#D#u#y#pbL#saZaPaI.K#G",
".Z#Eb.aPbR.KbKbK.KbG.w#G#0#u.w.1#P.y.S.e#z.w.ybA#H.wa0#u#b#D#bbgbO.1#Fa5bna5bg#D#z#u#F#s#D#2.e.4bl.O.4#5.r.p.Kbv#H#F.ya7bhbe#D.1#Ybq.SaY#Tbn.ybs#taI.g.Z.8#D.w.1a6bnbn.B#0#D#D#T.I.4bV#2aP.C.K#G",
"bw.UboaP.ZbL.ybS.K.M#s.w#G#G#G#D.1#G.S.e.K#0#FbKbCaza0.1.1.1#s#ia5.0#F#s#Da5aw#bbl#0.obVbV.M.e.4bl.O.SbOaJ.LbQ.ya0#F#D#8bhbe#D#sbe#B#sa7.4.1.e#F#taI#.#j.l#u.1.1.4.1.0a5b2#D#u.Z.UbLbS.SaP#t#z.r",
".Zbib..U.g.S.S.y#zbT.w#s#G#u#G#GbmbV.S.e.ybVaB#s.Fa0.wa0.0b4#z#ib2.0.oby#D.eaxaY.o#0aB#ubV#2.8.4blaP.e.Z.o.Lbubl.wbl#s#8bhbe.y#b.1.V.W.b#rbe#z.RbYaI#.#v#Tbl.1.1.e#G#7#0#0#D.R.Z.U.S#zbV.s#R#s.r",
"a6#E#4.YaP#h.4#ubL.4#z.w#G.1.0#D#G#G#G#u.8bG.8#zbC.1a0#zbV#b#z.J.w#GbbbVbAaB#O.1#F.w.8bSay.o#FaP#5a6.4#tbR.Z#zbV.1bdbS#DaAbVbQ#e#BaBaEbA.4#sbs#Dbl#tb1#A.Zbl#u.wbV.y#zay.0.1.y#2aT.4bVbVaZ.9#say",
".ZbDb.#3#.bF.y#D.T.S#z.w.1.1#G.0#0bc.1.1bVbA.y#b.1.1.w#D#Gbe#z.l#b#u#u#GbA.S.y#D#D#s.y#zay#F.KbtbZ#i.8.Z.saZbA#u.wbCa5#DaA.RbQbn#B.X#Y.4.KbV#u#D.o#tba#k.g#2#u#sbn.y#0#G.rbC#DaB.Z.S#u#sbY#t#s.r",
".ZbDbubBaP.TbA.1.KbA.K.w#P#b.1.0#0.wb4#P.wbVbnazbC#H#0#u.1be#DbA.wblay#0.4.S#z#D#0bV.8.1ay#u#saP.g#i.4.Z#tbDbK#7.1bCa5bnaW.R#zbn#BaB#Y#x.KbVbn.0#u.4#.a9#i.4#u.1bQ#FbS#G.0.1#D.S.O.4#u.w#tbYbQ.0",
".8.ZaKaIbNbRbK.wbL.SbA.w.w.1#0#D.B.1#P.1.1bV#H.z.1#b#G#F.1be#7#s#zbv#0.w.4bT#sbS#G#u.y#zblbl#saPaP.Z.e#tbR#t#s#G.w.1bV.1aW.y.Kbnbr.y#YaM#s.y.1#GbSbA.Z.YbW.4bV.w#b#FbS#z.0.1bV.y.O#TbVbV#t#t#s#G",
"#rbT#RbwbN.g.K.1#t.4.4.w.way.w#GbS#G#Hbc.1.1ao.z.1#PbV#uazazbs#bbU#c#0#0.8.6#z#u#G#F#u#Gbb#F.k#E#.bRbF.Z#t#R#b.1#GbC#zbn#N.R.eaV#sbKaYaZ#s.y.1#G#G.l.ZbW.U.gbA#G.w#2bS#G#u.1bVbSaq.Zbl#z#tbY#s.0",
"#z#x.s.CbNaP.Kbn#T.S#F#z#0bl#0bVay.w.1.1.1.1bc.z.1.w#z#u#bb4#u#b#s#c.w#zbA.a#z#D#G#FbV#Gbl#u.K.YbaaI.K#i.s.Sbe.z#s#GbSbC#r#D.KaV.RbQ.k.y#s.y.1#Gaybl.l.ZbN.ZbK#G.w.oa5#z#u.1#s#u.Oa6#FbSbY.Zbn#G",
"#rbT.9b..ObW.KbVbL.K#T.w#0#F#0ayb2.1bc.1bc.wbcao.1#G#zbV.w.1.ybQ#s#2.w#F#z#2#b#7aUbl#0.1#u#GbLaPbN.Hb.aC.saK#Y.z#0.1bV.1#r#D.eaYbAaLaBbK#Dbn#G#G#0#ubSbLa9#Z#z#G#Gbv#z#z#F#D#0.8.H.Z.8bA#tbY#s.1",
"a5#x#t#w.HaP.Kbn.K.K#2.w.wa8.wbS#0.w.1.1.1#Pbcbc.1bV#0bV#sbV.lbQ#z#u.wa##zbK#B#7#0#u#D#G#G.wbObN.p.n#h.E#taKbe.z.w.1bSbCbFbV.ebn.KaL#x.k.y#b#D.0#ubl#u.4#pbN.S.rbV#F#z#z.o.1.w#F.H.J.la5bY.s#sbc",
"#Dab#3.ybw#Z.K#zbnararaB#bbGbS#G.1bc.1.0bc.1.1.0ay.w#F#n#sbeai#JbA#D#b.4.e.1.z.o.B.w#u.1#DbQbLaPb#.Z.g.ZbDaka0.w#GbV#G#bbsbea7bnbK.q#4bA#G#G.0#Dar.3.y.R.U#9.Tbd#uay#r#z.MbVbe#Kb1bD#2.e#t#tbn.r",
".raR.ObSbw#Z.KbK.1ar.ebsaY#T.e.1#GbCbc#Hbc#H.1.1bV#0#F#nbVbe.5bV.lbd#b#N.8bnaz#u#u#0.r.1#ubK#t.Z#EaIbR#1aZapbe.1.1#u#D.w.ybebnb4bn.q.S.4ay#G#D.Rbla5#u.RaP#9.Tbd.r#z.ebU.M#Gbeaub1.g.4.K#t#t#s.0",
".raR.Oa5bwba.e.y.1ar.e.y#s.8bS.1#u#Ga0a0#Ha0.1#GbVbV#F#bbV.w#FbVbv#D#sbO#T.1az#0#u#G#G.1#ubK.Z.s#EaI#t.ga6#Y#b#G.1#DbV#DbK#BaVbeaVaY.S.4bV#G#D.y.la5#u.i.9aabOa7.r#G.e#z.M#0#b.y#Z.Z.8bQbL#t.w.0",
".r#o.ObVbwaS.K.S.1.ebA#D.w.e.e#D#G#G.1#Ha0bc.1.0.w#u#F#P#u#z#ubS#T.1bnar.Jbn.1.w#u#G.0.1#Dbn.9bw#E.Taq.gbT#bbn#uan#G#b#Dbnbqa7be.w#a.4#F.w#G.RaBaB.ebSbn.S#X.Zbd#GbV.4#z#F#0#b.S#Z.gbAbQbZ#R#sbc",
"#GabbwbS#3.p#4bK.wa5.e#Dbnby.lbnbV.1#Pbc.1.1#G#G#s#u#FbybV#u#0#G#T.w#D#r#x.1#Gby#G.1#G.1.1bQ.Gbw.Z#5.Za6.Zak.w#D#B#H#b.y#baLbn#a#b#baB.8.w#0bAbD#t.4bS#D.y.g#vbn#GbV.4#z#u#GbQ.y.L.ZbAbU.S.s.w.0",
"bcat.sbV.O#k.K#4#H.ebAbC.0.W.4#Day#G.w.1.0#D#G#u.w#u#u#bbV#F.w#u.l#H.X#r.lbebCa0#G#G#G.1.1#Y.G.C.gaIbW#i.g#s#D.0azbe.1.5#YbeaYbe.w.wbX.y.wbn.4bD.ubh#N#ubV.Zbgbn#G#z#2bQbV.w.w.y#Z.g.l#sbRbY#s.0",
"bm.Obw#z.Ob5#4.SbebSbA.1#Db0.Jbn.8#G.w.1#Gay#G#G#s#u#u#Y.1aBbe.R.ybe.5#z.Kan.0a0#0.1#Gbc#baLbI.C.gbR#EbWbT#z.y#Gb4anbnbsaLbn#a#a#H#b.ybA.w#zbOaPaT#V.l#FbVbOa3#s#0.e#2by#G.1bn.Rat.Z.4#z#t.s#s.0",
"bm#Z.s.w.Ob5bL#tb4.ebSbC#7bPbGbV.8ay.w.w#G#D#0#G#0#ubV#n#saibe.R#0an.b#z#z#lbd#0.1#Gbc.1#H#Y.EbZaIb..gbR.Sak#G#nb4.m#b.ybq.RaLbea0#b.y.y.wbS.4.g.O#XaA#F#DaraxbQa5.ebTbQ.1a0#G.y#3.Zbv.K#t#tbn.r",
"#G.sbw#z#2bNa2bSbV.8bQbVay#G.y.SblbS#Gay#G#0#G#ubybvbe#b#z.4.q.S#8#b#za8#Daz#7.w.w.r.1.1#H#s.GbBaBb.aP.CbAbn.1b4ananan###Bbn.mbnb4#b.ybv#GbnaP#ZadbN#jbSay.MbD.n.1#z.4#z#bbCbV#zbwaPa6#5bZbD.y.1",
".1#tbw#z.4a9#5bV#z.8.w#G.r#G#ubAbSbS#z#0#G.1#0bS#z.o#bbn.y#2akaB.1.w#0.y#0az#Dbn.w#G#G.1#baY.9.C#2bL.Y.s.S#DbC#Hazbeana7brbn#Wbnan#b.K#u.w#bbDb1#k#k.g.Saybv#xbw.w#z.4bK#P#G#G#z.s.g.g.KbR.ZbV.1",
".w.Sbw#z.8#..D#u#z.y#s#0bmbm#D.Ka5bS#G.w#GbS#G#0a5#F.1#DaB.4bQ#2#D.w#zbS.wbe#u#0.wbV#G.1#b.KaP.Z.ZaIbN#..S#u.1.1b4anbe#Hbebebrbnbe.w#z#u.wbn#E.LaG#ka9.4#zblaZ.sbV#z.4#z.w.0#D#s.H#i#x#5bZ.Z#s.1",
".wbLbw.K.8.O#5.8#GbS.w.1.r#G.wbVbS.8bV#0#G#ubVbV#u.yaY#D#2.ebQ#2.1bVa5.8.w.1bSbS.w#G#G#G#GbA.g#.bLbRba.O.K#D.1.1b4anb4brbebrbrbn#Hbn.KbS.w#Db#.fb6aG#kaq#za5.8.Z#zbV.S#s.way#GbnbBaP.g.TbR#t#s.1",
"bcaI#t.K.8.Oa2.8.w#z#z#0#Gbm.wbKa5blbVbV#G#u#ua5.y#D.wbVbGbQ#baB.wbSbA.4#b.RbVbA#0bV#G.wbVbY.Z.O.TbR#.aPbQ.ya0#b.z#Bb4anbe#B#Y.ibe#sbV#s#b.Rafb6#k#kadaP.e#zbA.4bS#s.R#s.wbS#D.1b7#i#x#4bR.9bV#G",
".w#4bw.e.8.OaI#u#z#z#z#G#G#G.w.4#za5bS#GbVbVbVbS.4bKbV.K.MbQaY#FazbVbAaB.q#F#GbS#G.1#Haz#saBbZ.H.e.gbabN.q#7#b.zan.manbebrbrbrbn.1#D#zbV.1.R.I.ha9.L#kbN.4#zbA.8ar#s.Rbn#0.B#GbnaI#E#i#5.s.Z#s#D",
"#GaKbR.e.yaqaI.8#z#sbK#0#G#G#saZa5bAbSbVbV#zbV#u.l#z.ybV#x.w#bbdazbV.K.Sbr.5.w#z.1bcbcaz#baBbZ.O.ebWbabi.qbs.zb4ananan.1#Bbe#Wbn.1.y.y#DbnaB#M.h.g.Oa9bibTbAbA.8.J#sbn.Raya5bVa7aI#E#i.KbR#t#s.0",
"#Gb.bBa5bA.Oa2#u.K.K.4#z#G#GbQbDbA.8bS.y#ubVbVaB#r.KbA#zbg#Y#b#Ha0bV#sbKaLa#be#0.1#H.1#B.1bY#taP.KaPaP.U#Y#7.z.zananan.ibqaV.mbn#b#DbV#z.1bXafac.ZaI#..I#i.lbA.8.J#s.i.ya5a5#DbdaIaC#ib.bR.4bQ#D",
"#Gb..ZbAbAbaaIbV#zbA.ybKbV#sbV#Far#z#Dblblar.8aB.y#zbR.KbG.w.1.z.wbV#T.can.Sbr#8b4.w#nb4#L#.bJ#R#z.g#3a9bq#F.z#H#f.mbe#sbe#D#L##bC#s.ybl#P#3.2.I#tbL.g#..paP.e.obTbQ#s#2.ybV.y#FbA.O.L.4.4bB#a.0",
"#Gb..ga5.eaPbB.ya5bl.8bS#0bn#s#u.Z#zbV#Fa8.8bA.y.8#4bR#1.8#G.1.1#PbVaAb4br.e.V#8an.waz#8#WbiaIbX.k#v.uajaL#Fbc.1#fbeanbnaLa7#La7#8#sbK.r.w.O.x.Y#t.e.gbWadbN#4bl#xbQ.K#2#ubVbV#D.lba.LbZaBbwak.0",
"#G.KaP.e#z.ObZ.y#za5bSbS#z.w#0bV.g.K#s#ua8.lbl.y#F.4.Z#i#z#G.0.w.w#DbOan.V.ebeanbr.w#bbCaLbi#..Sby.g.saObe#u#H.1#8be#B#Dbebr#L#8bc#sbKay.w.Y.LaT.4.K#taP#pbibA#u#2#s#zaB.ybV#sbV.4bababLaB.s.7.0",
"#Gbu.ga5#s.Obw.yby#0bSbS#0#zaybSbabZ.K#zbl.8bA#u.8.Saq.6#YbV#Ga0be.1bOanbrbU#8.zaza0.w#H#WaP#EbK#b.Z#R#XaV#0.z.1#b.1#B.R#Yan.m#8#H#JbK#G#0af.L.u.4.ebO.gbN.U#t#z.ybQbV.8.y.wbVbVaBba#Z.T.4.s.kbc",
"a0.7aP.e#z#Zbw#u#0#0#0#u#G#G#uaya9bR#4bVbSa5bS#F.K.T.Z.6b0ay#D#bbebV#T.m#8bU#D.1#8#0.1bebqaIbW#J#b#x#R#v#D#0bC#H#HbV#Bbs#B.mbe#ea0bK.K#G#0.I#Z#t#tbA.ebO#..Ua6bV#F.w#sbVbV#z#sbVbA.O#Z.ebYaT.7bc",
"#P.Q.ga5#sba.sbAbV.wbS#u#z#GbVbVbaaT.SbKa5a5bS#F.ebL#EbD#b#u#u#Y#H.1#Tbebd.W#7bd.1bS#0beaL.T#Ebn#Y.g.S#y#u#0aobe.w.1azbs#Bbrb4a7a0au.K#Pay.I.O.Z.4.y.ebLbR#..P#z.8bV#0bVbn.w#s#sbAbH#ZbL.4.9.kbc",
".1buaPa5#z.Y.O#ubV#sbVbV.1#z#D.wba.Z.4bA.8.e.KbA.4#t.UbL#s#zbbbe#b#b#ran.b.W#7.1#8#G#0#HbKb7.gaV#BbT.K#r#Dby#H.m.1.1be#u#B.1.wbe#P.S#z.wbSbi.n.ZbK.4.e.8aI.Z.6.K#u#GbV#0bV#s.w.w.e#Z#Z.K.4aT.k.1",
".rb..g.e#s.YaPbV#z.wbnbV.wbn#Dbn.OaP.4.y.4#N.K.K#T#t.U.Q#z.w#F#YazanaNan.bap#Ga0az#0#P#b#KbR.Z#aaz#ibK#h#u.wbC#B#b.1bebs#B#D#baLbcbA.k#P#G#E#S#2.K.8bAbL.Tbw.6.K#G#0.wbVbVbV#s#s.e.Oba.K.4aT.k.0"
};

NorwegianWoodStyle::NorwegianWoodStyle( int sbext ) : QWindowsStyle()
{
    if ( sbext >= 0 )
	setScrollBarExtent( sbext );
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::polish( QApplication *app)
{

    oldPalette = app->palette();

    // we simply create a nice QColorGroup with a couple of fancy wood
    // pixmaps here and apply to it all widgets

    QImage img(button_xpm);
    QImage orig = img;
    orig.detach();
    QPixmap button;
    button.convertFromImage(img);


    int i;
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.dark(120).rgb();
	img.setColor(i,rgb);
    }
    QPixmap mid;
    mid.convertFromImage(img);

    img = orig;
    img.detach();
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.light().rgb();
	img.setColor(i,rgb);
    }
    QPixmap light;
    light.convertFromImage(img);

    img = orig;
    img.detach();
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.dark(180).rgb();
	img.setColor(i,rgb);
    }
    QPixmap dark;
    dark.convertFromImage(img);

    
    QImage bgimage(polish_xpm);
    QPixmap background;
    background.convertFromImage(bgimage);

    img = bgimage;
    img.detach();
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.dark(180).rgb();
	img.setColor(i,rgb);
    }
    sunkenDark = new QPixmap;
    sunkenDark->convertFromImage(img);
    
    
    img = bgimage;
    img.detach();
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.light(130).rgb();
	img.setColor(i,rgb);
    }
    sunkenLight= new QPixmap;
    sunkenLight->convertFromImage(img);
    
    
    
    QPalette op(QColor(212,140,95));
    // QPalette op(white);
    QColorGroup active (op.normal().foreground(),
		     QBrush(op.normal().button(),button),
		     QBrush(op.normal().light(), light),
		     QBrush(op.normal().dark(), dark),
		     QBrush(op.normal().mid(), mid),
		     op.normal().text(),
		     Qt::white,
		     QColor(236,182,120),
		     QBrush(op.normal().background(), background)
		     );
    QColorGroup disabled (op.disabled().foreground(),
		     QBrush(op.disabled().button(),button),
		     QBrush(op.disabled().light(), light),
		     op.disabled().dark(),
		     QBrush(op.disabled().mid(), mid),
		     op.disabled().text(),
		     Qt::white,
		     QColor(236,182,120),
		     QBrush(op.disabled().background(), background)
		     );

   app->setPalette(QPalette(active, disabled, active), TRUE );

}

void NorwegianWoodStyle::unPolish( QApplication *app)
{
    app->setPalette(oldPalette, TRUE);
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::polish( QWidget* w)
{

    // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.

    if (w->inherits("QTipLabel") || w->inherits("QLCDNumber") ){
	return;
    }

    if ( !w->isTopLevel() ) {
	if ( w->inherits("QPushButton")
	     || w->inherits("QToolButton")
	     || w->inherits("QGroupBox")
	     || w->inherits("QTabWidget")
	     || w->inherits("QComboBox") ) {
	    w->setAutoMask( TRUE );
	    return;
	}
 	if (w->inherits("QLabel")
	    || w->inherits("QButton") ) {
	    w->setBackgroundOrigin( QWidget::ParentOrigin );
 	}
    }
}

void NorwegianWoodStyle::unPolish( QWidget* w)
{
    // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.
    if (w->inherits("QTipLabel") || w->inherits("QLCDNumber") ){
	return;
    }
    if ( !w->isTopLevel() ) {
	if ( w->inherits("QPushButton")
	     || w->inherits("QToolButton")
	     || w->inherits("QGroupBox")
	     || w->inherits("QTabWidget")
	     || w->inherits("QComboBox") ) {
	    w->setAutoMask( FALSE );
	    return;
	}
 	if (w->inherits("QLabel")
	    || w->inherits("QButton") ) {
	    w->setBackgroundOrigin( QWidget::WidgetOrigin );
 	}
    }
}

static void drawroundrect( QPainter *p, QCOORD x, QCOORD y,
			   QCOORD w, QCOORD h, QCOORD d )
{
    int rx = (200*d)/w;
    int ry = (200*d)/h;
    p->drawRoundRect( x, y, w, h, rx, ry );
}




static QRegion roundRectRegion( const QRect& g, int r )
{
    QPointArray a;
    a.setPoints( 8, g.x()+r, g.y(), g.right()-r, g.y(),
		 g.right(), g.y()+r, g.right(), g.bottom()-r,
		 g.right()-r, g.bottom(), g.x()+r, g.bottom(),
		 g.x(), g.bottom()-r, g.x(), g.y()+r );  
    QRegion reg( a );
    int d = r*2-1;
    reg += QRegion( g.x(),g.y(),r*2,r*2, QRegion::Ellipse );
    reg += QRegion( g.right()-d,g.y(),r*2,r*2, QRegion::Ellipse );
    reg += QRegion( g.x(),g.bottom()-d,r*2,r*2, QRegion::Ellipse );
    reg += QRegion( g.right()-d,g.bottom()-d,r*2,r*2, QRegion::Ellipse );
    return reg;
}




static int get_combo_extra_width( int h, int *return_awh=0 )
{
    int awh;
    if ( h < 8 ) {
	awh = 6;
    } else if ( h < 14 ) {
	awh = h - 2;
    } else {
	awh = h/2;
    }
    if ( return_awh )
	*return_awh = awh;
    return awh*3/2;
}


static void get_combo_parameters( const QRect &r,
				  int &ew, int &awh, int &ax,
				  int &ay, int &sh, int &dh,
				  int &sy )
{
    ew = get_combo_extra_width( r.height(), &awh );

    sh = (awh+3)/4;
    if ( sh < 3 )
	sh = 3;
    dh = sh/2 + 1;

    ay = r.y() + (r.height()-awh-sh-dh)/2;
    if ( ay < 0 ) {
	//panic mode
	ay = 0;
	sy = r.height();
    } else {
	sy = ay+awh+dh;
    }
    ax = r.x() + r.width() - ew +(ew-awh)/2;
}


void NorwegianWoodStyle::drawComboButton( QPainter *p, int x, int y, int w, int h,
				    const QColorGroup &g,
				    bool /* sunken */,
				    bool editable,
				    bool /*enabled */,
				    const QBrush *fb )
{
    QBrush fill = fb ? *fb : g.brush( QColorGroup::Button );

    int awh, ax, ay, sh, sy, dh, ew;
    get_combo_parameters( buttonRect(x,y,w,h), ew, awh, ax, ay, sh, dh, sy );

    drawButton( p, x, y, w, h, g, FALSE, &fill );

    qDrawArrow( p, DownArrow, MotifStyle, FALSE,
		ax, ay, awh, awh, g, TRUE );

    p->setPen( g.light() );
    p->drawLine( ax, sy, ax+awh-1, sy );
    p->drawLine( ax, sy, ax, sy+sh-1 );
    p->setPen( g.dark() );
    p->drawLine( ax+1, sy+sh-1, ax+awh-1, sy+sh-1 );
    p->drawLine( ax+awh-1, sy+1, ax+awh-1, sy+sh-1 );

    if ( 0 && editable ) {
	QRect r( comboButtonRect(x-1,y-1,w+2,h+2) );
	qDrawShadePanel( p, r, g, TRUE, 1, &fill );
    }
}


static inline int buttonthickness( int d )
{ return  d > 20 ? 5 : ( d < 10 ? 2: 3 ); }

enum { PointUp, PointDown, PointLeft, PointRight };


void NorwegianWoodStyle::drawSemicircleButton( QPainter *p, const QRect &r, 
					       int dir, bool sunken,
					       const QColorGroup &g )
{
    int b = scrollBarExtent().height() > 20 ? 3 : 2;
    
    QRegion extrn(  r.x(),   r.y(),   r.width(),     r.height(),     QRegion::Ellipse );
    QRegion intern( r.x()+b, r.y()+b, r.width()-2*b, r.height()-2*b, QRegion::Ellipse );
    int w2 = r.width()/2;
    int h2 = r.height()/2;

    int bug = 1; //off-by-one somewhere!!!???
    
    switch( dir ) {
    case PointRight:
	extrn +=  QRegion( r.x(),  r.y(),   w2,     r.height() );
	intern += QRegion( r.x()+b,r.y()+b, w2-2*b, r.height()-2*b );
	break;
    case PointLeft:
	extrn +=  QRegion( r.x()+w2,  r.y(),   w2,     r.height() );
	intern += QRegion( r.x()+w2+b,r.y()+b, w2-2*b, r.height()-2*b );
	break;
    case PointUp:
	extrn +=  QRegion( r.x(),  r.y()+h2,   r.width(),     h2 );
	intern += QRegion( r.x()+b,r.y()+h2+b, r.width()-2*b-bug, h2-2*b-bug );
	break;
    case PointDown:
	extrn +=  QRegion( r.x(),  r.y(),   r.width(),     h2 );
	intern += QRegion( r.x()+b,r.y()+b, r.width()-2*b-bug, h2-2*b-bug );
	break;
    }

    extrn = extrn - intern;
    QPointArray a;
    a.setPoints( 3, r.x(), r.y(), r.x(), r.bottom(), r.right(), r.top() );
    
    QRegion oldClip = p->clipRegion();
    p->setClipRegion( intern );
    p->fillRect( r, g.brush( QColorGroup::Button ) );

    p->setClipRegion( QRegion(a)&extrn );
    p->fillRect( r, sunken ? g.dark() : g.light() );

    a.setPoints( 3, r.right(), r.bottom(), r.x(), r.bottom(), r.right(), r.top() );
    p->setClipRegion( QRegion(a) &  extrn );
    p->fillRect( r, sunken ? g.light() : g.dark() );    

    p->setClipRegion( oldClip );
}



void NorwegianWoodStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb,
						int sliderStart, uint controls,
						uint activeControl )
{
    QWindowsStyle::drawScrollBarControls( p, sb, sliderStart, controls & ~(AddLine|SubLine),
					activeControl & ~(AddLine|SubLine) );
    bool horz = sb->orientation() == QScrollBar::Horizontal;
    int b = 2;
    int w = horz ? sb->height() : sb->width();

    QColorGroup g = sb->colorGroup();
    
    if ( controls & AddLine ) {
	bool sunken = activeControl & AddLine;
	QRect r( b, b, w-2*b, w-2*b ) ;
	if ( horz ) {
	    r.moveBy( sb->width() - w, 0 );
	    drawSemicircleButton( p, r, PointRight, sunken, g );
	} else {
	    r.moveBy( 0, sb->height() - w );
	    drawSemicircleButton( p, r, PointDown, sunken, g );
	}
    } 
    if ( controls & SubLine ) {
	bool sunken = activeControl & SubLine;
	QRect r( b, b, w-2*b, w-2*b ) ;
	if ( horz ) {
	    drawSemicircleButton( p, r, PointLeft, sunken, g );
	} else {
	    drawSemicircleButton( p, r, PointUp, sunken, g );
	}
    }
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::drawButton( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &g, bool sunken, const QBrush* fill)
{
    int d = QMIN(w,h)/2;

    int b = buttonthickness( d );
    
    QRegion internR = roundRectRegion( QRect(x+b, y+b, w-2*b, h-2*b), d-b );
    //    qDrawShadePanel( p, x, y, w, h, g, sunken, 5);
    QPen oldPen = p->pen();

    QBrush brush( fill ? *fill : (sunken ? g.brush( QColorGroup::Mid ) :
                                          g.brush( QColorGroup::Button )));
    p->setClipRegion( internR );
    p->fillRect( x, y, w, h, brush );

    
    int e = QMIN( w, h )/2; 
    
    QPoint p2(x+w-1-e,y+e);
    QPoint p3(x+e, y+h-1-e);
    
    QPointArray a;
    a.setPoints( 5, x,y, x+w-1, y, p2.x(),p2.y(), p3.x(),p3.y(), x, y+h-1 );
    
    p->setClipRegion( QRegion( a )- internR );
    
    p->fillRect( x,y,w,h, (sunken ? QBrush( g.dark(), *sunkenDark )
			   : g.brush( QColorGroup::Light ) ));

    
    a.setPoint( 0, x+w-1, y+w-1 );
    p->setClipRegion( QRegion( a ) - internR );
    
    p->fillRect( x,y,w,h, (sunken ? QBrush( g.light(), *sunkenLight )
			   : g.brush( QColorGroup::Dark )));
    
    /*    
    QBrush oldBrush = p->brush();

    p->setPen( NoPen );
    p->setBrush( fill ? *fill : (sunken ? g.brush( QColorGroup::Mid ) :
                                          g.brush( QColorGroup::Button )));
    drawroundrect( p, x+3, y+3, w-6, h-6, 5 );
    p->setBrush( oldBrush );
    */
        p->setClipRegion( internR );
    p->setClipping( FALSE );
    p->setPen( g.foreground() );
    drawroundrect( p, x, y, w, h, d );
    p->setPen( oldPen );
}

/*!
  Reimplementation from QStyle

  \sa QStyle
  */
void NorwegianWoodStyle::drawBevelButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &g, bool sunken, const QBrush* fill)
{
    QWindowsStyle::drawBevelButton(p, x, y, w, h, g, sunken, fill);
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::drawPushButton( QPushButton* btn, QPainter *p)
{
    QColorGroup g = btn->colorGroup();
    int x1, y1, x2, y2;

    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

    p->setPen( g.foreground() );
    p->setBrush( QBrush(g.button(),NoBrush) );

    QBrush fill;
    if ( btn->isDown() )
	fill = g.brush( QColorGroup::Mid );
    else if ( btn->isOn() )
	fill = QBrush( g.mid(), Dense4Pattern );
    else
	fill = g.brush( QColorGroup::Button );	

    if ( btn->isDefault() ) {
	x1 += 2;
	y1 += 2;
	x2 -= 2;
	y2 -= 2;
    }
	
    drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, btn->isOn() || btn->isDown(), &fill);
    
    if ( btn->isDefault() ) {
	QPen pen( Qt::black, 4 );
	pen.setCapStyle( Qt::RoundCap );
	pen.setJoinStyle( Qt::RoundJoin );
	p->setPen( pen );
	drawroundrect( p, x1-1, y1-1, x2-x1+3, y2-y1+3, 8 );
    }


    if ( btn->isMenuButton() ) {
	int dx = (y1-y2-4)/3;
	drawArrow( p, DownArrow, FALSE,
		   x2 - dx, dx, y1, y2 - y1,
		   g, btn->isEnabled() );
    }

    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );

}


/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::drawPushButtonLabel( QPushButton* btn, QPainter *p)
{
    QRect r = btn->rect();
    int x, y, w, h;
    r.rect( &x, &y, &w, &h );

    int x1, y1, x2, y2;
    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates
    int dx = 0;
    int dy = 0;
    if ( btn->isMenuButton() )
	dx = (y2-y1) / 3;
    if ( dx || dy )
	p->translate( dx, dy );

    x += 2;  y += 2;  w -= 4;  h -= 4;
    QColorGroup g = btn->colorGroup();
    drawItem( p, x, y, w, h,
	      AlignCenter|ShowPrefix,
	      g, btn->isEnabled(),
	      btn->pixmap(), btn->text(), -1,
	      (btn->isDown() || btn->isOn())?&btn->colorGroup().brightText():&btn->colorGroup().buttonText());

    if ( dx || dy )
	p->translate( -dx, -dy );
}

/*!
  Reimplementation from QStyle
 */
QRect NorwegianWoodStyle::buttonRect( int x, int y, int w, int h){
    int d = QMIN(w,h)/2;
    int b = buttonthickness( d );

    d -= b;
    b++;
    
    if ( w<h )
	return QRect(x+b, y+d, w-2*b, h-2*d);
    else
	return QRect(x+d, y+b, w-2*d, h-2*b);
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::drawButtonMask( QPainter *p, int x, int y, int w, int h)
{
    int d = QMIN(w,h)/2;
    drawroundrect( p, x, y, w, h, d );
}

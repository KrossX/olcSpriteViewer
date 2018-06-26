/*
OneLoneCoder.com - Command Line Game Engine
"Who needs a frame buffer?" - @Javidx9

License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.

Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9

GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE

From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE

Cheers!

Background
~~~~~~~~~~
If you've seen any of my videos - I like to do things using the windows console. It's quick
and easy, and allows you to focus on just the code that matters - ideal when you're 
experimenting. Thing is, I have to keep doing the same initialisation and display code
each time, so this class wraps that up.

Author
~~~~~~
Twitter: @javidx9	http://twitter.com/javidx9
Blog:				http://www.onelonecoder.com
YouTube:			http://www.youtube.com/javidx9

Videos:
~~~~~~
Original:				https://youtu.be/cWc0hgYwZyc
Added mouse support:	https://youtu.be/tdqc9hZhHxM
Beginners Guide:		https://youtu.be/u5BhrA8ED0o

Shout Outs!
~~~~~~~~~~~
Thanks to cool people who helped with testing, bug-finding and fixing!
wowLinh, JavaJack59, idkwid, kingtatgi, Return Null, CPP Guy

Last Updated: 18/03/2018

Usage:
~~~~~~
This class is abstract, so you must inherit from it. Override the OnUserCreate() function
with all the stuff you need for your application (for thready reasons it's best to do
this in this function and not your class constructor). Override the OnUserUpdate(float fElapsedTime)
function with the good stuff, it gives you the elapsed time since the last call so you
can modify your stuff dynamically. Both functions should return true, unless you need
the application to close.

	int main()
	{
		// Use olcConsoleGameEngine derived app
		OneLoneCoder_Example game;

		// Create a console with resolution 160x100 characters
		// Each character occupies 8x8 pixels
		game.ConstructConsole(160, 100, 8, 8);

		// Start the engine!
		game.Start();

		return 0;
	}

Input is also handled for you - interrogate the m_keys[] array with the virtual
keycode you want to know about. bPressed is set for the frame the key is pressed down
in, bHeld is set if the key is held down, bReleased is set for the frame the key
is released in. The same applies to mouse! m_mousePosX and Y can be used to get
the current cursor position, and m_mouse[1..5] returns the mouse buttons.

The draw routines treat characters like pixels. By default they are set to white solid
blocks - but you can draw any unicode character, using any of the colours listed below.

There may be bugs! 

See my other videos for examples!
http://www.youtube.com/javidx9

Lots of programs to try:
http://www.github.com/OneLoneCoder/videos

Chat on the Discord server:
https://discord.gg/WhwHUMV

Be bored by Twitch:
http://www.twitch.tv/javidx9

*/

#pragma once

#ifndef UNICODE
#pragma message("Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode. Thanks! For now, I'll try enabling it for you - Javidx9")
#define UNICODE
#define _UNICODE
#endif

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")

#include <windows.h>
#include <gl/gl.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>
using namespace std;

#define GL_GENERATE_MIPMAP                0x8191
#define GL_GENERATE_MIPMAP_HINT           0x8192

typedef BOOL  (WINAPI wglSwapInterval_t) (int interval);
wglSwapInterval_t *wglSwapInterval;

enum COLOUR
{
	FG_BLACK		= 0x0000,
	FG_DARK_BLUE    = 0x0001,	
	FG_DARK_GREEN   = 0x0002,
	FG_DARK_CYAN    = 0x0003,
	FG_DARK_RED     = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW  = 0x0006,
	FG_GREY			= 0x0007, // Thanks MS :-/
	FG_DARK_GREY    = 0x0008,
	FG_BLUE			= 0x0009,
	FG_GREEN		= 0x000A,
	FG_CYAN			= 0x000B,
	FG_RED			= 0x000C,
	FG_MAGENTA		= 0x000D,
	FG_YELLOW		= 0x000E,
	FG_WHITE		= 0x000F,
	BG_BLACK		= 0x0000,
	BG_DARK_BLUE	= 0x0010,
	BG_DARK_GREEN	= 0x0020,
	BG_DARK_CYAN	= 0x0030,
	BG_DARK_RED		= 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW	= 0x0060,
	BG_GREY			= 0x0070,
	BG_DARK_GREY	= 0x0080,
	BG_BLUE			= 0x0090,
	BG_GREEN		= 0x00A0,
	BG_CYAN			= 0x00B0,
	BG_RED			= 0x00C0,
	BG_MAGENTA		= 0x00D0,
	BG_YELLOW		= 0x00E0,
	BG_WHITE		= 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

// Based on the PxPlus IBM CGA Font from 
// "The Ultimate Oldschool PC Font Pack" http://int10h.org/oldschool-pc-fonts/
// Rendered into an image (256x56) and converted to char array
//
// The Ultimate Oldschool PC Font Pack is licensed under a Creative Commons
// Attribution-ShareAlike 4.0 International License.
//
// You should have received a copy of the license along with this work. If
//  not, see < http://creativecommons.org/licenses/by-sa/4.0/ >.
//
//                                                           (c) 2016 VileR
//

// (256x256x8)
unsigned char pxplus_ibm_cga[65536];

char pxplus_ibm_cga_enc[8509+1] = 
"0mWOkP0780et0q?qqedtH40@HHVtq106HP10000000001stOLm320q??33cI<Kqk707?Vbbf0e3?HP130@26t3@YKtW?Le16WKFVeK6tshh7qnWIKh10sn76eHe0Vfh"
"t04mttrlOsehe2j[dVfPfWtlOHHVlk006HPfOt=etsn70r>a?sqlOq<a@r>3?<HqnOe76VQb=sn76s1S13Hktq0@VWh17[n36WKFVhPQ3Vhh1en70HhQOqP1?HeeOTq"
"?604mt8P0280etq<q<sreIK70@qH6khm76HP1000000000sn700e170q?0tp167<06000600P70q?0000000000003kH330eQ1HH00000000V?<mQ7mq37tmQ700060"
"HP70mQ=kmhHkH03<H630000e<V3h<3?3He<h<33<e00<<30NHhO3<373H06qe000006hf0<eHh730h<he036q36e0030HS7Hm606Pftt0e?0eeN<e17h0h7HmQ?00e0"
"00360e00t133p0P1He3300006q6360hOe<33h030<H00He0000P=OHk<0e03Vf030ee0Wfe8h0h<hee<He03<q3300030H33Sn60HH000e00<4P?tqS7mnQ7<mQ3<H0"
"60H03000000000000000060000000000000000000000000P?<q3?OqlOq<S7mMk3S=67tmf?Nqh<h<kHhqW73m120<V7VIV=6ITIhe0<VIeMWISIhHk<b<h<S=k<SI"
"P1He10p=SI3HV5F<e<<0S=6qlKSIk<VI03h<hHk<C<6e06k0eNhmh0VnQ73q33emQ1tblHs<S?<ee<ha67NPQ1HPfH0al?V=PIFHfLhee<kHiJh=V1pH36<<h<tf13<"
"I0<H0003<SIVISA6Hk<<<SIV=kHkHP7V=33hmfMkePI606600P7hq3?Oql3q=S7NLkOS=67?PhINmf?<<kHNqW70n10000000000000000000000000000000000000"
"000000et<0e10P30L0e1<0h1>0000000000200000000mPf1c1030H00e0P=0H000H03000000000e0000000003Hee>8PQ76m1<NHPKkm0<Vfe<Omf>cbS?s<h<S=k"
"<te06<00700S?hmh<?<SK<0S=<ql<hHk<c=03h<hJk<C6700>0H30sHk0hqS1hH63em13t=h<V=SINee<hq77he03He00S1e<V=h<3HP?Vfe<keeJh<S?sH0<a<S7tI"
"S?Ve06<0eH0mk>NmV7?0hIN<hIN<k<NH0<?q16cfP=S1h?mPf10q7000000000O000N0000000?P7000000000O00000000006He10hP1OSn3?0000sq?7<mP3H0Pt0"
"003L0ee3O030P16k<k<HH<01I3h00@@0H33HP13Vb=00mP=h<kH<3000mW9sm16L0@FkH600i40kq33<0PIK300<HSIh<SI<0063qeHt0P=0D4OhqSOE107<H060HVl"
"0003LeqkpfS10Pf06<63HH3051PIe0@C0003Nm00VQ=60m10Vf>hO?00HmlIsq36L0@Fsf<<0D50000000P?H3060mh<kK6ph006HqhH<Pf<044000001100t0000H0"
"k003000eqhHd7000600030m10s00000P?000000003000L00000<q33e0Pnf?Sf0ONL0>N<h1mmf<Oqh0PnhO33P?7PS7hPh3Ne17h007<Hh<00e<0000h0P=0P13S1"
"06S=600<300He<kH30NH30h<e?tqh?Nm10NHk<qm17LeS=h=h<0<h<s<hHSnf<SnfOhHP16H03<m13dMSIhHS=VffNh<h<h<SIKqlOhqhOh<S7NmQ7Ne03<ePItHk<S"
"=VIkqk<h<h<Nmh<S=k?h<k?hPQ16HP1<e03<Hh>qmQ=kehHW=h<h<336<kHS=h<S=hLeqh?tqS7NmQ7O<36<e17H0P?NmQ7Nmf3h000000000P70000000000000000"
"00000000000000007PSOt<3300e1mml<7eQ?hP1000P700030000N00070000<<00e00000033000<60qrf1m<h?he0@7Ph<hPS1hmQ7qmQ7Nmd7Nm1?NmP3Lm0<000"
"000000m3000000m30e03He03<e<e<hHk<<e06<mh7NmQ7Nmf?h=h<h<h<V=S?seW?smSt3qh?sr33<P13h<h<h<h<h0eJh<h<h<SIh<h<V=h<h<S73<P13e03Hee<h<"
"h<h<h<<Lk<h<h<smS?sn7tsnWOs3S7NeS7Nm1?Nmf<NmQ7Nm13smWOsn7<603000000000L000000000000000000000@000000mf3OqS?h<30007>sf16<HV=hal7e"
"qh?h<33<q70kHSOLHVI00P7Nm10q0eeke30qeQ7cI3O00P7N0006107L<a=qeS7N0P7hmQINe30Vn1?0q3HV1h?NqS7tmQ5NqS7q10O0<3<N0h?e<e<Vfh0hH6?VflK"
"sHe<6<S1hmf<6<S1cIPKtmh<s<S?3<e06<e03HPIVIk<NqS7tmf?FqS7t<l<h=h<hqh<h<SIhHV1V=SI6HVIk<S13He06<PA3He0VnSIs<SOhm73sfS7qe3?Ne3?ter"
"7crS7tmf?NqW?tm1Oee7<00000P1m000000000000000000000000e07000e70q16He3LNLWI7qh7tqf<hm13<0eqSQ7NWM00?pq3>q=k?e30VQf<Ntd100000mQ7<0"
"000<60<haI60PIaI03kMW1Vnk0c1P1VrP7>mQ30003>mQ3S=6<0HSIWI036eP16Hk0h<l<hHSOke03<eP7>e03<eeHS13<NHS=6ePA<HP16<l<V=h?cIVK<e03<e03<"
"e03<qkHh0S=NmQA<H636IPAVn3Os<SIVI63<e03<e03<e03drk<hHV=kH63tnQI6H6Oe03<hLVIWnQ7NmQ7NmQ7Nmf>dnf<WMVItn16Hql3t1e7Lq10000000000000"
"mP30000>0m1>m0000mP30000>qP3eQhH0Hh<6<60t1eHhe60q10Ler30kHSlme17N0036e000L60Lmf0W100tmS70eV=0000V107L<30kHh<0eV3qLk7drfI00eKOe1"
"0L0070<SOtaSIpqh>6mS?0HP?<Le3d=hNhqk7Oal<kmQ=NHS7hbQIcnSKVn633LP7<<036feNh<l<p=h<h=hHh<k<S=h<pnSIkHP?VQQ7NL06NePI<<l<S=hLh<hHhH"
"h<k<S=hHh6kHeI?HS1h03>m<3<NqW7S=36<<k<h<k<Lm17NeQ7qnlI?P16WrP7OqQ7Nq1000000me10000mP10000000000000>m00000000e17<S=tPe<hq32t0e?0"
"<h<<e0K0<30LeQ7N<3eme03k<37NeA;<mQ7be00t0e?Nm10<00Kh0P=k<h<0q70t1eONHh<seP?te03s<30h0000<30h0e<h00000e<hqh<t<h?6He0<e@;sq33h<h<"
"h<h<h<h<h<h<[=k<h<C6ITA6ITQ1Hm13<e03<mf<h<h<h<h<h<h<h<hO[nf<Ne43<f0C<He<em1;<e23<=h<h<h<h<h<h<h<hLlO<m33VIRIVHV96mf7HPQ7Hm1>NmW"
"7snQONmW7snQ?SIS7emfOtql?trh300e3>0000000000000000000e17000P700000000000L0q3?q07LeQS70q320q32KmhH<e10cf60H0A60iGKrP=0he0300P?0="
"30be00<eQ30m30k0e>0e033l30=jF6Hm760m1Os?W?6m33smV7k00000P=0000<@00VQ13aQ1?KeQ7eH3<p=733e03p<hH00000e10000000PIHe3OHPf6e<S?tntK["
"QQ7<ee4h000000000000e000m70<e66Haf<t<h<hLkIh033aaS7000000000000<000V10CaQf6>mf<s=WtsmS7Om16cf000000000600P100PI0e7Kqm0000000000"
"1e03<e00t00000000P30000000000000MiF7@ee?tPeOt=37NL62S=kOLqh?trh<8<kHL<h<HeP=bIheNHk<LHiHhH33VffMW=V=hHVIb<S?SbV=00000<6KSQf<V=P"
"=F4h<SfP=kqlK0<k<Vf03haV=[=V7hmV?SQhHH<S?3<V7HqhO<mfHtbW?S=S?HeP7[fQ?Sfe<p<eH@<66tHk0SI1Ch<63k<kJh1eHhH03<eP?LPeH<mf<>H34kPf<V="
"eH6Ik<kePIS=kHSIh<6H63<PP=8H33<ah0LPhMe<h?3qlOt=37NLkHS=kOL<h3tnQ78<62lnQ7cn3000000000000000000000000000000000000000000e03@0P70"
"m10t0P700P300e?00000000000000e<he0300ee0<30h0@60<300a100e00000000000000000000@7<0PKh<S1se@7eee<e<h<6mf?NmS?t<h>hakH<<S7hL33Vbh7"
"Nmf0<LS7<a1?h<3?h<h<Sb13haV7[b63h<h<kePIh<33hm06k<33?Hh<hPf<h<h0hee<[feJ[fe<h<S=<Hk>Omf<30S=hee6hqW7e<h<Omh<<<S?NmhJ<<h<hH36qmk"
"0hmQ?hHS7H<h<3eP=N<h0Pn16NPe<8H36NmQ700000<P700P7000000e00e100<0>00008002000000060ef1kq1LqmQ7hPl7Kq167ef<<ql?tflO[nfHLL6NS1P7k0"
"0060PIhe00eXf66L600<S76IVIVIiJh<kHVfkMS=hHtrW?t=P1<m1<oaQ?k<kHh<S1VITIFa6<h=W=VrlJOqWA6IVIOe03<0SFtHV7h=k<hmS?6HV7sefNpnQItbk<S"
"nQ7VIi0He03hXjFVIhNs=h?VIV1VIfJeqkKkHkJ[rQ=6ITI6Hk<<ee<JbUIVr6Hh<SIVIPI6bk<WMVIV=V=3efOtMh3qmQ7Nmf>pLkIWnh?hqh??qlO[nfHSMkHS1e0"
"000000000000000000000e000000S10000000000hef?teh?hPeHh<hJ[MeH3qf<s10000000000000000e<k<SIVjg<s<k<hakJ6<k0eak<000000000000L0000<h"
"HhHk0<<hJk<h<[bV1S=0H[=S7trhOqnfJs<kHWQlHt<k<s<03sa67hmhJ[nhKOelKs0S1VIVIhakHh=W=aMl<S=S13e0<[ff<eakJVbj<PbV=smS?6Hk?sPhNpnQIt="
"S=hHPI<<hJk<3<[bVIK=3<[=h<VIV1V=eJSrkKkHkJhef<?eS7NmhHt1hOtnhKOqf<hnk?tqeONaV?WMkIS=6000000000800H000H0000000000000<60000000000"
"0000000000000000000000000e1kH000003h0300H0L000000000P000000000000000000?07000000000?0e<Nqh>Nqh<s<k<hakJ7<k0s<SONmQ10mS?>m0<OaQ1"
"W=h<hHk<b<hJk<h<[bV1S=eH[=h<hHhOS=03<0S6KHS=t<h<V=03ha67hmhJ[nhKOPlKsqh?cIk3Ne03eXh?cnf<h<S?heP?sHh<eakJVbj<SbV=3<PI6<6<<ee<JbU"
"Ik<S7hHP7N032Sr7<trW?drQ?h<W7NLk3sqQ7N<h>pLkI0000?000Oe10P100P10000000000e0000000N0000000<00H0mlHOefOtm0?t<3?trh0ta7><qSItqhHSr"
"h?[M000q7H0H6<e03HHPfH[16HeqSIs166VIVIVIVIP1kJ0<h<6q70V13<e066H<kJP13<P=VIePfHVIVIke6HPbk<h<S160eJe03<SQ1<S=6<e03HS=6<H<VIcn67m"
"H6H[bh<hHP10<3<m0hHH0hHS10<e0hHS136SI6HPf0<k0kItmh<6H00S13?e<66H<k<003<H<kHePQ=k06H<0V5P=h=eqh3?0eHtI3<SQ13Sr10eq33tb7<Nef7Pr73"
"tI0HO0e7<000000000000000000<00000030000H0e006000t<h<hPQI00007L0Le=kHmL0000000000He00<HVI0<SIVIVIHHVKtq70000000030L0000000003<00"
"3hH60hHVIVI63hah<k<kHS=kHSQf<000000000eP10Pf<h0PIVIVIV100H<P=[bkJ[bkJs<h<tmS?tt?00000000000PIV16000063HhOtrlOtrl<N<300000000000"
"0000000HVIP1000Pf0kLlOlrlMt=33s0000000t300<0000H60WI6H0000H<P=SIhHk<V=Nm1<0000000000030000V100000000000000000000000q1000000q?00"
"H0000e<0e0300000P1K6000Vr?0s060Hm17se17<0P700000000<e0000e<<H33HHPI00eHe0076H0<kH330H30000000e?tP100aQ1KP13<H6000060mQ3NP17qqS7"
"k00000000e03q006<0000H06V1000e00sPQ=<H3<<0P=<mP3Hm17<e0?0PQ10000<ePI00006006>e13Le130HS3HP176H03tPfk0a=00006600000e000000000000"
"00e03<mQ3Ne030a=0I3000000V1eH04P?000000000000<H06sPQ=<e000000000000000m300<600000000000P7Nm06>e1000000q77O00?>CeqOh103?qe3?qe7h"
"qe??qf?t<0H00000006Ih<OIkH>L[sk0eHS=VISI6Kc=kHSfkHaf0?000000P5d<CD?<h6t[oHN<h<h<h<hHh?hLh<kLS=6mW?Le130mQ1OEU1Naf>:>k<KalNtbttq"
"htNqb7OqblOH0S=ke00FqeHEr0O><?0kq3tLh<hqeh?qef7LtrlLK06He1?tmQ1V=OAVnb5h0P=3HarVIVMkIWMsnWMkIWM<P13k0330qe?Sj7?Kle<0LW7S=qe3?qe"
"3?qkKdrkKd=0He07Le0060em000L0000000He17Le17Le17Le17Le1000P10HP17PQ07tr30P10q00P7e1000PQ1sm3000760P16H@2?qH3<LHeHh00<00300<3kc1h"
"?<eeHSQ00HS10H036VnWOemW=3<V10P10eml0hPr>t1P1H<kLL00kH006tQftHPQ?K?k7Sfe?<P1<K?e<H006te03[bW=tP160P1<sI66s=hkS=eH60P1HLhk3<36cf"
"00HHeHd=k0HPftHP1?Tm7?hmlH6<k<0<00kmlOhaf>tr300<kIS=06K0060P10qPQ7<q77Sr30100?00e<K0P10qh?smhO0Pf60P100006t3P100eH00000P3000P30"
"00000000000Hm00H000HP16H006H00K00000000He6KHe6KHe6KHe6K00000006HP160P160e6000000006af66af66af66af6000000P16HP10HPftaQ?0qt10tQ?K"
"at1KdQ?Kat1Kdr?0t300HP16H006H00KH003H00HHe63He6HHe63He6H0000mt1sOPt7ttttth6sqhs7tr6sqht7tr7sahs7drkttt>6H000HP160P10aQ1KaQ1Ka10"
"000000P1KaQ1KaQ1KaQ160006HP10H00KHe6KHe6K00000000He6KHe6KHe6KHP100P16H0060e66af66af6000000006af66af66af66af66afkt00et?0?00D5000"
"0000@0004200ett3POsQ1KaQ1Kar?00ip3e30AZ2000000HLetef1?qqte04mtthkpthkpt300tt0qT8BGqm30000?Oq?OkHVOWW97Uc=0a106a1et00er?0?00X>?k"
"eS?0mlOsrlH2nlerJCPttttttttd300tts3e300GfS=qHSOtt1?qIS@s=Ldkipe0000He6K00ettt0qT@TZqm3?smlt7P1LLHVOWW97IN>00006af60ttOd?0?0Ri50"
"0000s1@00042qeht33@Pt3000P1Ka1etttt3e300Z2000000000000000qtt0mWOHe3q8eQ=8edt0h7O33000000000000000000000000ekV1>2sq77<K<ekIFV000"
"0000000000000000000000eSIeh17tn3ts3V1Vr90000000000000000000000000WKVdsqlOtfPfPrlK?3000000000000000000000000enq<hOtnS?<HqHVIkn00"
"00000000000000000000000e36hmhJLeQ3VKSIVr?0000000000000000000000000Kpl<8P028qeIqqspW300000000000000000000000006Hm17L0007<0600et0";

unsigned char_decode_custom_base64(unsigned c6) 
{
	if(c6 < 97) return c6 - '0';
	else if(c6 < 127) return c6 + 44 - 'a';
	else return c6 - 74;
}

void font_decode_custom_base64(void) 
{
	int i, o = 0;
	
	for(i = 0; i < 8509; i++) 
	{
		unsigned c = char_decode_custom_base64(pxplus_ibm_cga_enc[i]);
		pxplus_ibm_cga[o++] = ((c>>0)&1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c>>1)&1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c>>2)&1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c>>3)&1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c>>4)&1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c>>5)&1) * 0xFF;
	}
}


void GetFontCoords(int id, int *x, int *y)
{
	switch(id)
	{
		case 0:     *x=0;     *y=0;   break;
		case 1:     *x=8;     *y=0;   break;
		case 2:     *x=16;    *y=0;   break;
		case 3:     *x=24;    *y=0;   break;
		case 4:     *x=32;    *y=0;   break;
		case 5:     *x=40;    *y=0;   break;
		case 6:     *x=48;    *y=0;   break;
		case 7:     *x=56;    *y=0;   break;
		case 8:     *x=64;    *y=0;   break;
		case 9:     *x=72;    *y=0;   break;
		case 10:    *x=80;    *y=0;   break;
		case 11:    *x=88;    *y=0;   break;
		case 12:    *x=96;    *y=0;   break;
		case 13:    *x=104;   *y=0;   break;
		case 14:    *x=112;   *y=0;   break;
		case 15:    *x=120;   *y=0;   break;
		case 16:    *x=128;   *y=0;   break;
		case 17:    *x=136;   *y=0;   break;
		case 18:    *x=144;   *y=0;   break;
		case 19:    *x=152;   *y=0;   break;
		case 20:    *x=160;   *y=0;   break;
		case 21:    *x=168;   *y=0;   break;
		case 22:    *x=176;   *y=0;   break;
		case 23:    *x=184;   *y=0;   break;
		case 24:    *x=192;   *y=0;   break;
		case 25:    *x=200;   *y=0;   break;
		case 26:    *x=208;   *y=0;   break;
		case 27:    *x=216;   *y=0;   break;
		case 28:    *x=224;   *y=0;   break;
		case 29:    *x=232;   *y=0;   break;
		case 30:    *x=240;   *y=0;   break;
		case 31:    *x=248;   *y=0;   break;
		case 32:    *x=0;     *y=8;   break;
		case 33:    *x=8;     *y=8;   break;
		case 34:    *x=16;    *y=8;   break;
		case 35:    *x=24;    *y=8;   break;
		case 36:    *x=32;    *y=8;   break;
		case 37:    *x=40;    *y=8;   break;
		case 38:    *x=48;    *y=8;   break;
		case 39:    *x=56;    *y=8;   break;
		case 40:    *x=64;    *y=8;   break;
		case 41:    *x=72;    *y=8;   break;
		case 42:    *x=80;    *y=8;   break;
		case 43:    *x=88;    *y=8;   break;
		case 44:    *x=96;    *y=8;   break;
		case 45:    *x=104;   *y=8;   break;
		case 46:    *x=112;   *y=8;   break;
		case 47:    *x=120;   *y=8;   break;
		case 48:    *x=128;   *y=8;   break;
		case 49:    *x=136;   *y=8;   break;
		case 50:    *x=144;   *y=8;   break;
		case 51:    *x=152;   *y=8;   break;
		case 52:    *x=160;   *y=8;   break;
		case 53:    *x=168;   *y=8;   break;
		case 54:    *x=176;   *y=8;   break;
		case 55:    *x=184;   *y=8;   break;
		case 56:    *x=192;   *y=8;   break;
		case 57:    *x=200;   *y=8;   break;
		case 58:    *x=208;   *y=8;   break;
		case 59:    *x=216;   *y=8;   break;
		case 60:    *x=224;   *y=8;   break;
		case 61:    *x=232;   *y=8;   break;
		case 62:    *x=240;   *y=8;   break;
		case 63:    *x=248;   *y=8;   break;
		case 64:    *x=0;     *y=16;  break;
		case 65:    *x=8;     *y=16;  break;
		case 66:    *x=16;    *y=16;  break;
		case 67:    *x=24;    *y=16;  break;
		case 68:    *x=32;    *y=16;  break;
		case 69:    *x=40;    *y=16;  break;
		case 70:    *x=48;    *y=16;  break;
		case 71:    *x=56;    *y=16;  break;
		case 72:    *x=64;    *y=16;  break;
		case 73:    *x=72;    *y=16;  break;
		case 74:    *x=80;    *y=16;  break;
		case 75:    *x=88;    *y=16;  break;
		case 76:    *x=96;    *y=16;  break;
		case 77:    *x=104;   *y=16;  break;
		case 78:    *x=112;   *y=16;  break;
		case 79:    *x=120;   *y=16;  break;
		case 80:    *x=128;   *y=16;  break;
		case 81:    *x=136;   *y=16;  break;
		case 82:    *x=144;   *y=16;  break;
		case 83:    *x=152;   *y=16;  break;
		case 84:    *x=160;   *y=16;  break;
		case 85:    *x=168;   *y=16;  break;
		case 86:    *x=176;   *y=16;  break;
		case 87:    *x=184;   *y=16;  break;
		case 88:    *x=192;   *y=16;  break;
		case 89:    *x=200;   *y=16;  break;
		case 90:    *x=208;   *y=16;  break;
		case 91:    *x=216;   *y=16;  break;
		case 92:    *x=224;   *y=16;  break;
		case 93:    *x=232;   *y=16;  break;
		case 94:    *x=240;   *y=16;  break;
		case 95:    *x=248;   *y=16;  break;
		case 96:    *x=0;     *y=24;  break;
		case 97:    *x=8;     *y=24;  break;
		case 98:    *x=16;    *y=24;  break;
		case 99:    *x=24;    *y=24;  break;
		case 100:   *x=32;    *y=24;  break;
		case 101:   *x=40;    *y=24;  break;
		case 102:   *x=48;    *y=24;  break;
		case 103:   *x=56;    *y=24;  break;
		case 104:   *x=64;    *y=24;  break;
		case 105:   *x=72;    *y=24;  break;
		case 106:   *x=80;    *y=24;  break;
		case 107:   *x=88;    *y=24;  break;
		case 108:   *x=96;    *y=24;  break;
		case 109:   *x=104;   *y=24;  break;
		case 110:   *x=112;   *y=24;  break;
		case 111:   *x=120;   *y=24;  break;
		case 112:   *x=128;   *y=24;  break;
		case 113:   *x=136;   *y=24;  break;
		case 114:   *x=144;   *y=24;  break;
		case 115:   *x=152;   *y=24;  break;
		case 116:   *x=160;   *y=24;  break;
		case 117:   *x=168;   *y=24;  break;
		case 118:   *x=176;   *y=24;  break;
		case 119:   *x=184;   *y=24;  break;
		case 120:   *x=192;   *y=24;  break;
		case 121:   *x=200;   *y=24;  break;
		case 122:   *x=208;   *y=24;  break;
		case 123:   *x=216;   *y=24;  break;
		case 124:   *x=224;   *y=24;  break;
		case 125:   *x=232;   *y=24;  break;
		case 126:   *x=240;   *y=24;  break;
		case 127:   *x=248;   *y=24;  break;
		case 160:   *x=0;     *y=32;  break;
		case 161:   *x=8;     *y=32;  break;
		case 162:   *x=16;    *y=32;  break;
		case 163:   *x=24;    *y=32;  break;
		case 164:   *x=32;    *y=32;  break;
		case 165:   *x=40;    *y=32;  break;
		case 166:   *x=48;    *y=32;  break;
		case 167:   *x=56;    *y=32;  break;
		case 168:   *x=64;    *y=32;  break;
		case 169:   *x=72;    *y=32;  break;
		case 170:   *x=80;    *y=32;  break;
		case 171:   *x=88;    *y=32;  break;
		case 172:   *x=96;    *y=32;  break;
		case 173:   *x=104;   *y=32;  break;
		case 174:   *x=112;   *y=32;  break;
		case 175:   *x=120;   *y=32;  break;
		case 176:   *x=128;   *y=32;  break;
		case 177:   *x=136;   *y=32;  break;
		case 178:   *x=144;   *y=32;  break;
		case 179:   *x=152;   *y=32;  break;
		case 180:   *x=160;   *y=32;  break;
		case 181:   *x=168;   *y=32;  break;
		case 182:   *x=176;   *y=32;  break;
		case 183:   *x=184;   *y=32;  break;
		case 184:   *x=192;   *y=32;  break;
		case 185:   *x=200;   *y=32;  break;
		case 186:   *x=208;   *y=32;  break;
		case 187:   *x=216;   *y=32;  break;
		case 188:   *x=224;   *y=32;  break;
		case 189:   *x=232;   *y=32;  break;
		case 190:   *x=240;   *y=32;  break;
		case 191:   *x=248;   *y=32;  break;
		case 192:   *x=0;     *y=40;  break;
		case 193:   *x=8;     *y=40;  break;
		case 194:   *x=16;    *y=40;  break;
		case 195:   *x=24;    *y=40;  break;
		case 196:   *x=32;    *y=40;  break;
		case 197:   *x=40;    *y=40;  break;
		case 198:   *x=48;    *y=40;  break;
		case 199:   *x=56;    *y=40;  break;
		case 200:   *x=64;    *y=40;  break;
		case 201:   *x=72;    *y=40;  break;
		case 202:   *x=80;    *y=40;  break;
		case 203:   *x=88;    *y=40;  break;
		case 204:   *x=96;    *y=40;  break;
		case 205:   *x=104;   *y=40;  break;
		case 206:   *x=112;   *y=40;  break;
		case 207:   *x=120;   *y=40;  break;
		case 208:   *x=128;   *y=40;  break;
		case 209:   *x=136;   *y=40;  break;
		case 210:   *x=144;   *y=40;  break;
		case 211:   *x=152;   *y=40;  break;
		case 212:   *x=160;   *y=40;  break;
		case 213:   *x=168;   *y=40;  break;
		case 214:   *x=176;   *y=40;  break;
		case 215:   *x=184;   *y=40;  break;
		case 216:   *x=192;   *y=40;  break;
		case 217:   *x=200;   *y=40;  break;
		case 218:   *x=208;   *y=40;  break;
		case 219:   *x=216;   *y=40;  break;
		case 220:   *x=224;   *y=40;  break;
		case 221:   *x=232;   *y=40;  break;
		case 222:   *x=240;   *y=40;  break;
		case 223:   *x=248;   *y=40;  break;
		case 224:   *x=0;     *y=48;  break;
		case 225:   *x=8;     *y=48;  break;
		case 226:   *x=16;    *y=48;  break;
		case 227:   *x=24;    *y=48;  break;
		case 228:   *x=32;    *y=48;  break;
		case 229:   *x=40;    *y=48;  break;
		case 230:   *x=48;    *y=48;  break;
		case 231:   *x=56;    *y=48;  break;
		case 232:   *x=64;    *y=48;  break;
		case 233:   *x=72;    *y=48;  break;
		case 234:   *x=80;    *y=48;  break;
		case 235:   *x=88;    *y=48;  break;
		case 236:   *x=96;    *y=48;  break;
		case 237:   *x=104;   *y=48;  break;
		case 238:   *x=112;   *y=48;  break;
		case 239:   *x=120;   *y=48;  break;
		case 240:   *x=128;   *y=48;  break;
		case 241:   *x=136;   *y=48;  break;
		case 242:   *x=144;   *y=48;  break;
		case 243:   *x=152;   *y=48;  break;
		case 244:   *x=160;   *y=48;  break;
		case 245:   *x=168;   *y=48;  break;
		case 246:   *x=176;   *y=48;  break;
		case 247:   *x=184;   *y=48;  break;
		case 248:   *x=192;   *y=48;  break;
		case 249:   *x=200;   *y=48;  break;
		case 250:   *x=208;   *y=48;  break;
		case 251:   *x=216;   *y=48;  break;
		case 252:   *x=224;   *y=48;  break;
		case 253:   *x=232;   *y=48;  break;
		case 254:   *x=240;   *y=48;  break;
		case 255:   *x=248;   *y=48;  break;
		case 256:   *x=0;     *y=56;  break;
		case 257:   *x=8;     *y=56;  break;
		case 258:   *x=16;    *y=56;  break;
		case 259:   *x=24;    *y=56;  break;
		case 260:   *x=32;    *y=56;  break;
		case 261:   *x=40;    *y=56;  break;
		case 262:   *x=48;    *y=56;  break;
		case 263:   *x=56;    *y=56;  break;
		case 264:   *x=64;    *y=56;  break;
		case 265:   *x=72;    *y=56;  break;
		case 266:   *x=80;    *y=56;  break;
		case 267:   *x=88;    *y=56;  break;
		case 268:   *x=96;    *y=56;  break;
		case 269:   *x=104;   *y=56;  break;
		case 270:   *x=112;   *y=56;  break;
		case 271:   *x=120;   *y=56;  break;
		case 272:   *x=128;   *y=56;  break;
		case 273:   *x=136;   *y=56;  break;
		case 274:   *x=144;   *y=56;  break;
		case 275:   *x=152;   *y=56;  break;
		case 276:   *x=160;   *y=56;  break;
		case 277:   *x=168;   *y=56;  break;
		case 278:   *x=176;   *y=56;  break;
		case 279:   *x=184;   *y=56;  break;
		case 280:   *x=192;   *y=56;  break;
		case 281:   *x=200;   *y=56;  break;
		case 282:   *x=208;   *y=56;  break;
		case 283:   *x=216;   *y=56;  break;
		case 284:   *x=224;   *y=56;  break;
		case 285:   *x=232;   *y=56;  break;
		case 286:   *x=240;   *y=56;  break;
		case 287:   *x=248;   *y=56;  break;
		case 288:   *x=0;     *y=64;  break;
		case 289:   *x=8;     *y=64;  break;
		case 290:   *x=16;    *y=64;  break;
		case 291:   *x=24;    *y=64;  break;
		case 292:   *x=32;    *y=64;  break;
		case 293:   *x=40;    *y=64;  break;
		case 294:   *x=48;    *y=64;  break;
		case 295:   *x=56;    *y=64;  break;
		case 296:   *x=64;    *y=64;  break;
		case 297:   *x=72;    *y=64;  break;
		case 298:   *x=80;    *y=64;  break;
		case 299:   *x=88;    *y=64;  break;
		case 300:   *x=96;    *y=64;  break;
		case 301:   *x=104;   *y=64;  break;
		case 302:   *x=112;   *y=64;  break;
		case 303:   *x=120;   *y=64;  break;
		case 304:   *x=128;   *y=64;  break;
		case 305:   *x=136;   *y=64;  break;
		case 306:   *x=144;   *y=64;  break;
		case 307:   *x=152;   *y=64;  break;
		case 308:   *x=160;   *y=64;  break;
		case 309:   *x=168;   *y=64;  break;
		case 310:   *x=176;   *y=64;  break;
		case 311:   *x=184;   *y=64;  break;
		case 312:   *x=192;   *y=64;  break;
		case 313:   *x=200;   *y=64;  break;
		case 314:   *x=208;   *y=64;  break;
		case 315:   *x=216;   *y=64;  break;
		case 316:   *x=224;   *y=64;  break;
		case 317:   *x=232;   *y=64;  break;
		case 318:   *x=240;   *y=64;  break;
		case 319:   *x=248;   *y=64;  break;
		case 320:   *x=0;     *y=72;  break;
		case 321:   *x=8;     *y=72;  break;
		case 322:   *x=16;    *y=72;  break;
		case 323:   *x=24;    *y=72;  break;
		case 324:   *x=32;    *y=72;  break;
		case 325:   *x=40;    *y=72;  break;
		case 326:   *x=48;    *y=72;  break;
		case 327:   *x=56;    *y=72;  break;
		case 328:   *x=64;    *y=72;  break;
		case 329:   *x=72;    *y=72;  break;
		case 330:   *x=80;    *y=72;  break;
		case 331:   *x=88;    *y=72;  break;
		case 332:   *x=96;    *y=72;  break;
		case 333:   *x=104;   *y=72;  break;
		case 334:   *x=112;   *y=72;  break;
		case 335:   *x=120;   *y=72;  break;
		case 336:   *x=128;   *y=72;  break;
		case 337:   *x=136;   *y=72;  break;
		case 338:   *x=144;   *y=72;  break;
		case 339:   *x=152;   *y=72;  break;
		case 340:   *x=160;   *y=72;  break;
		case 341:   *x=168;   *y=72;  break;
		case 342:   *x=176;   *y=72;  break;
		case 343:   *x=184;   *y=72;  break;
		case 344:   *x=192;   *y=72;  break;
		case 345:   *x=200;   *y=72;  break;
		case 346:   *x=208;   *y=72;  break;
		case 347:   *x=216;   *y=72;  break;
		case 348:   *x=224;   *y=72;  break;
		case 349:   *x=232;   *y=72;  break;
		case 350:   *x=240;   *y=72;  break;
		case 351:   *x=248;   *y=72;  break;
		case 352:   *x=0;     *y=80;  break;
		case 353:   *x=8;     *y=80;  break;
		case 354:   *x=16;    *y=80;  break;
		case 355:   *x=24;    *y=80;  break;
		case 356:   *x=32;    *y=80;  break;
		case 357:   *x=40;    *y=80;  break;
		case 358:   *x=48;    *y=80;  break;
		case 359:   *x=56;    *y=80;  break;
		case 360:   *x=64;    *y=80;  break;
		case 361:   *x=72;    *y=80;  break;
		case 362:   *x=80;    *y=80;  break;
		case 363:   *x=88;    *y=80;  break;
		case 364:   *x=96;    *y=80;  break;
		case 365:   *x=104;   *y=80;  break;
		case 366:   *x=112;   *y=80;  break;
		case 367:   *x=120;   *y=80;  break;
		case 368:   *x=128;   *y=80;  break;
		case 369:   *x=136;   *y=80;  break;
		case 370:   *x=144;   *y=80;  break;
		case 371:   *x=152;   *y=80;  break;
		case 372:   *x=160;   *y=80;  break;
		case 373:   *x=168;   *y=80;  break;
		case 374:   *x=176;   *y=80;  break;
		case 375:   *x=184;   *y=80;  break;
		case 376:   *x=192;   *y=80;  break;
		case 377:   *x=200;   *y=80;  break;
		case 378:   *x=208;   *y=80;  break;
		case 379:   *x=216;   *y=80;  break;
		case 380:   *x=224;   *y=80;  break;
		case 381:   *x=232;   *y=80;  break;
		case 382:   *x=240;   *y=80;  break;
		case 383:   *x=248;   *y=80;  break;
		case 402:   *x=0;     *y=88;  break;
		case 417:   *x=8;     *y=88;  break;
		case 439:   *x=16;    *y=88;  break;
		case 506:   *x=24;    *y=88;  break;
		case 507:   *x=32;    *y=88;  break;
		case 508:   *x=40;    *y=88;  break;
		case 509:   *x=48;    *y=88;  break;
		case 510:   *x=56;    *y=88;  break;
		case 511:   *x=64;    *y=88;  break;
		case 536:   *x=72;    *y=88;  break;
		case 537:   *x=80;    *y=88;  break;
		case 538:   *x=88;    *y=88;  break;
		case 539:   *x=96;    *y=88;  break;
		case 593:   *x=104;   *y=88;  break;
		case 632:   *x=112;   *y=88;  break;
		case 710:   *x=120;   *y=88;  break;
		case 711:   *x=128;   *y=88;  break;
		case 713:   *x=136;   *y=88;  break;
		case 728:   *x=144;   *y=88;  break;
		case 729:   *x=152;   *y=88;  break;
		case 730:   *x=160;   *y=88;  break;
		case 731:   *x=168;   *y=88;  break;
		case 732:   *x=176;   *y=88;  break;
		case 733:   *x=184;   *y=88;  break;
		case 894:   *x=192;   *y=88;  break;
		case 900:   *x=200;   *y=88;  break;
		case 901:   *x=208;   *y=88;  break;
		case 902:   *x=216;   *y=88;  break;
		case 903:   *x=224;   *y=88;  break;
		case 904:   *x=232;   *y=88;  break;
		case 905:   *x=240;   *y=88;  break;
		case 906:   *x=248;   *y=88;  break;
		case 908:   *x=0;     *y=96;  break;
		case 910:   *x=8;     *y=96;  break;
		case 911:   *x=16;    *y=96;  break;
		case 912:   *x=24;    *y=96;  break;
		case 913:   *x=32;    *y=96;  break;
		case 914:   *x=40;    *y=96;  break;
		case 915:   *x=48;    *y=96;  break;
		case 916:   *x=56;    *y=96;  break;
		case 917:   *x=64;    *y=96;  break;
		case 918:   *x=72;    *y=96;  break;
		case 919:   *x=80;    *y=96;  break;
		case 920:   *x=88;    *y=96;  break;
		case 921:   *x=96;    *y=96;  break;
		case 922:   *x=104;   *y=96;  break;
		case 923:   *x=112;   *y=96;  break;
		case 924:   *x=120;   *y=96;  break;
		case 925:   *x=128;   *y=96;  break;
		case 926:   *x=136;   *y=96;  break;
		case 927:   *x=144;   *y=96;  break;
		case 928:   *x=152;   *y=96;  break;
		case 929:   *x=160;   *y=96;  break;
		case 931:   *x=168;   *y=96;  break;
		case 932:   *x=176;   *y=96;  break;
		case 933:   *x=184;   *y=96;  break;
		case 934:   *x=192;   *y=96;  break;
		case 935:   *x=200;   *y=96;  break;
		case 936:   *x=208;   *y=96;  break;
		case 937:   *x=216;   *y=96;  break;
		case 938:   *x=224;   *y=96;  break;
		case 939:   *x=232;   *y=96;  break;
		case 940:   *x=240;   *y=96;  break;
		case 941:   *x=248;   *y=96;  break;
		case 942:   *x=0;     *y=104; break;
		case 943:   *x=8;     *y=104; break;
		case 944:   *x=16;    *y=104; break;
		case 945:   *x=24;    *y=104; break;
		case 946:   *x=32;    *y=104; break;
		case 947:   *x=40;    *y=104; break;
		case 948:   *x=48;    *y=104; break;
		case 949:   *x=56;    *y=104; break;
		case 950:   *x=64;    *y=104; break;
		case 951:   *x=72;    *y=104; break;
		case 952:   *x=80;    *y=104; break;
		case 953:   *x=88;    *y=104; break;
		case 954:   *x=96;    *y=104; break;
		case 955:   *x=104;   *y=104; break;
		case 956:   *x=112;   *y=104; break;
		case 957:   *x=120;   *y=104; break;
		case 958:   *x=128;   *y=104; break;
		case 959:   *x=136;   *y=104; break;
		case 960:   *x=144;   *y=104; break;
		case 961:   *x=152;   *y=104; break;
		case 962:   *x=160;   *y=104; break;
		case 963:   *x=168;   *y=104; break;
		case 964:   *x=176;   *y=104; break;
		case 965:   *x=184;   *y=104; break;
		case 966:   *x=192;   *y=104; break;
		case 967:   *x=200;   *y=104; break;
		case 968:   *x=208;   *y=104; break;
		case 969:   *x=216;   *y=104; break;
		case 970:   *x=224;   *y=104; break;
		case 971:   *x=232;   *y=104; break;
		case 972:   *x=240;   *y=104; break;
		case 973:   *x=248;   *y=104; break;
		case 974:   *x=0;     *y=112; break;
		case 976:   *x=8;     *y=112; break;
		case 1012:  *x=16;    *y=112; break;
		case 1024:  *x=24;    *y=112; break;
		case 1025:  *x=32;    *y=112; break;
		case 1026:  *x=40;    *y=112; break;
		case 1027:  *x=48;    *y=112; break;
		case 1028:  *x=56;    *y=112; break;
		case 1029:  *x=64;    *y=112; break;
		case 1030:  *x=72;    *y=112; break;
		case 1031:  *x=80;    *y=112; break;
		case 1032:  *x=88;    *y=112; break;
		case 1033:  *x=96;    *y=112; break;
		case 1034:  *x=104;   *y=112; break;
		case 1035:  *x=112;   *y=112; break;
		case 1036:  *x=120;   *y=112; break;
		case 1037:  *x=128;   *y=112; break;
		case 1038:  *x=136;   *y=112; break;
		case 1039:  *x=144;   *y=112; break;
		case 1040:  *x=152;   *y=112; break;
		case 1041:  *x=160;   *y=112; break;
		case 1042:  *x=168;   *y=112; break;
		case 1043:  *x=176;   *y=112; break;
		case 1044:  *x=184;   *y=112; break;
		case 1045:  *x=192;   *y=112; break;
		case 1046:  *x=200;   *y=112; break;
		case 1047:  *x=208;   *y=112; break;
		case 1048:  *x=216;   *y=112; break;
		case 1049:  *x=224;   *y=112; break;
		case 1050:  *x=232;   *y=112; break;
		case 1051:  *x=240;   *y=112; break;
		case 1052:  *x=248;   *y=112; break;
		case 1053:  *x=0;     *y=120; break;
		case 1054:  *x=8;     *y=120; break;
		case 1055:  *x=16;    *y=120; break;
		case 1056:  *x=24;    *y=120; break;
		case 1057:  *x=32;    *y=120; break;
		case 1058:  *x=40;    *y=120; break;
		case 1059:  *x=48;    *y=120; break;
		case 1060:  *x=56;    *y=120; break;
		case 1061:  *x=64;    *y=120; break;
		case 1062:  *x=72;    *y=120; break;
		case 1063:  *x=80;    *y=120; break;
		case 1064:  *x=88;    *y=120; break;
		case 1065:  *x=96;    *y=120; break;
		case 1066:  *x=104;   *y=120; break;
		case 1067:  *x=112;   *y=120; break;
		case 1068:  *x=120;   *y=120; break;
		case 1069:  *x=128;   *y=120; break;
		case 1070:  *x=136;   *y=120; break;
		case 1071:  *x=144;   *y=120; break;
		case 1072:  *x=152;   *y=120; break;
		case 1073:  *x=160;   *y=120; break;
		case 1074:  *x=168;   *y=120; break;
		case 1075:  *x=176;   *y=120; break;
		case 1076:  *x=184;   *y=120; break;
		case 1077:  *x=192;   *y=120; break;
		case 1078:  *x=200;   *y=120; break;
		case 1079:  *x=208;   *y=120; break;
		case 1080:  *x=216;   *y=120; break;
		case 1081:  *x=224;   *y=120; break;
		case 1082:  *x=232;   *y=120; break;
		case 1083:  *x=240;   *y=120; break;
		case 1084:  *x=248;   *y=120; break;
		case 1085:  *x=0;     *y=128; break;
		case 1086:  *x=8;     *y=128; break;
		case 1087:  *x=16;    *y=128; break;
		case 1088:  *x=24;    *y=128; break;
		case 1089:  *x=32;    *y=128; break;
		case 1090:  *x=40;    *y=128; break;
		case 1091:  *x=48;    *y=128; break;
		case 1092:  *x=56;    *y=128; break;
		case 1093:  *x=64;    *y=128; break;
		case 1094:  *x=72;    *y=128; break;
		case 1095:  *x=80;    *y=128; break;
		case 1096:  *x=88;    *y=128; break;
		case 1097:  *x=96;    *y=128; break;
		case 1098:  *x=104;   *y=128; break;
		case 1099:  *x=112;   *y=128; break;
		case 1100:  *x=120;   *y=128; break;
		case 1101:  *x=128;   *y=128; break;
		case 1102:  *x=136;   *y=128; break;
		case 1103:  *x=144;   *y=128; break;
		case 1104:  *x=152;   *y=128; break;
		case 1105:  *x=160;   *y=128; break;
		case 1106:  *x=168;   *y=128; break;
		case 1107:  *x=176;   *y=128; break;
		case 1108:  *x=184;   *y=128; break;
		case 1109:  *x=192;   *y=128; break;
		case 1110:  *x=200;   *y=128; break;
		case 1111:  *x=208;   *y=128; break;
		case 1112:  *x=216;   *y=128; break;
		case 1113:  *x=224;   *y=128; break;
		case 1114:  *x=232;   *y=128; break;
		case 1115:  *x=240;   *y=128; break;
		case 1116:  *x=248;   *y=128; break;
		case 1117:  *x=0;     *y=136; break;
		case 1118:  *x=8;     *y=136; break;
		case 1119:  *x=16;    *y=136; break;
		case 1168:  *x=24;    *y=136; break;
		case 1169:  *x=32;    *y=136; break;
		case 1470:  *x=40;    *y=136; break;
		case 1488:  *x=48;    *y=136; break;
		case 1489:  *x=56;    *y=136; break;
		case 1490:  *x=64;    *y=136; break;
		case 1491:  *x=72;    *y=136; break;
		case 1492:  *x=80;    *y=136; break;
		case 1493:  *x=88;    *y=136; break;
		case 1494:  *x=96;    *y=136; break;
		case 1495:  *x=104;   *y=136; break;
		case 1496:  *x=112;   *y=136; break;
		case 1497:  *x=120;   *y=136; break;
		case 1498:  *x=128;   *y=136; break;
		case 1499:  *x=136;   *y=136; break;
		case 1500:  *x=144;   *y=136; break;
		case 1501:  *x=152;   *y=136; break;
		case 1502:  *x=160;   *y=136; break;
		case 1503:  *x=168;   *y=136; break;
		case 1504:  *x=176;   *y=136; break;
		case 1505:  *x=184;   *y=136; break;
		case 1506:  *x=192;   *y=136; break;
		case 1507:  *x=200;   *y=136; break;
		case 1508:  *x=208;   *y=136; break;
		case 1509:  *x=216;   *y=136; break;
		case 1510:  *x=224;   *y=136; break;
		case 1511:  *x=232;   *y=136; break;
		case 1512:  *x=240;   *y=136; break;
		case 1513:  *x=248;   *y=136; break;
		case 1514:  *x=0;     *y=144; break;
		case 1520:  *x=8;     *y=144; break;
		case 1521:  *x=16;    *y=144; break;
		case 1522:  *x=24;    *y=144; break;
		case 1523:  *x=32;    *y=144; break;
		case 1524:  *x=40;    *y=144; break;
		case 7451:  *x=48;    *y=144; break;
		case 7462:  *x=56;    *y=144; break;
		case 7464:  *x=64;    *y=144; break;
		case 7808:  *x=72;    *y=144; break;
		case 7809:  *x=80;    *y=144; break;
		case 7810:  *x=88;    *y=144; break;
		case 7811:  *x=96;    *y=144; break;
		case 7812:  *x=104;   *y=144; break;
		case 7813:  *x=112;   *y=144; break;
		case 7839:  *x=120;   *y=144; break;
		case 7922:  *x=128;   *y=144; break;
		case 7923:  *x=136;   *y=144; break;
		case 8208:  *x=144;   *y=144; break;
		case 8210:  *x=152;   *y=144; break;
		case 8211:  *x=160;   *y=144; break;
		case 8212:  *x=168;   *y=144; break;
		case 8213:  *x=176;   *y=144; break;
		case 8215:  *x=184;   *y=144; break;
		case 8216:  *x=192;   *y=144; break;
		case 8217:  *x=200;   *y=144; break;
		case 8218:  *x=208;   *y=144; break;
		case 8219:  *x=216;   *y=144; break;
		case 8220:  *x=224;   *y=144; break;
		case 8221:  *x=232;   *y=144; break;
		case 8222:  *x=240;   *y=144; break;
		case 8223:  *x=248;   *y=144; break;
		case 8224:  *x=0;     *y=152; break;
		case 8225:  *x=8;     *y=152; break;
		case 8226:  *x=16;    *y=152; break;
		case 8230:  *x=24;    *y=152; break;
		case 8231:  *x=32;    *y=152; break;
		case 8240:  *x=40;    *y=152; break;
		case 8242:  *x=48;    *y=152; break;
		case 8243:  *x=56;    *y=152; break;
		case 8245:  *x=64;    *y=152; break;
		case 8249:  *x=72;    *y=152; break;
		case 8250:  *x=80;    *y=152; break;
		case 8252:  *x=88;    *y=152; break;
		case 8254:  *x=96;    *y=152; break;
		case 8255:  *x=104;   *y=152; break;
		case 8256:  *x=112;   *y=152; break;
		case 8260:  *x=120;   *y=152; break;
		case 8276:  *x=128;   *y=152; break;
		case 8308:  *x=136;   *y=152; break;
		case 8309:  *x=144;   *y=152; break;
		case 8310:  *x=152;   *y=152; break;
		case 8311:  *x=160;   *y=152; break;
		case 8312:  *x=168;   *y=152; break;
		case 8313:  *x=176;   *y=152; break;
		case 8314:  *x=184;   *y=152; break;
		case 8315:  *x=192;   *y=152; break;
		case 8319:  *x=200;   *y=152; break;
		case 8321:  *x=208;   *y=152; break;
		case 8322:  *x=216;   *y=152; break;
		case 8323:  *x=224;   *y=152; break;
		case 8324:  *x=232;   *y=152; break;
		case 8325:  *x=240;   *y=152; break;
		case 8326:  *x=248;   *y=152; break;
		case 8327:  *x=0;     *y=160; break;
		case 8328:  *x=8;     *y=160; break;
		case 8329:  *x=16;    *y=160; break;
		case 8330:  *x=24;    *y=160; break;
		case 8331:  *x=32;    *y=160; break;
		case 8355:  *x=40;    *y=160; break;
		case 8356:  *x=48;    *y=160; break;
		case 8359:  *x=56;    *y=160; break;
		case 8362:  *x=64;    *y=160; break;
		case 8364:  *x=72;    *y=160; break;
		case 8453:  *x=80;    *y=160; break;
		case 8467:  *x=88;    *y=160; break;
		case 8470:  *x=96;    *y=160; break;
		case 8482:  *x=104;   *y=160; break;
		case 8486:  *x=112;   *y=160; break;
		case 8494:  *x=120;   *y=160; break;
		case 8528:  *x=128;   *y=160; break;
		case 8529:  *x=136;   *y=160; break;
		case 8531:  *x=144;   *y=160; break;
		case 8532:  *x=152;   *y=160; break;
		case 8533:  *x=160;   *y=160; break;
		case 8534:  *x=168;   *y=160; break;
		case 8535:  *x=176;   *y=160; break;
		case 8536:  *x=184;   *y=160; break;
		case 8537:  *x=192;   *y=160; break;
		case 8538:  *x=200;   *y=160; break;
		case 8539:  *x=208;   *y=160; break;
		case 8540:  *x=216;   *y=160; break;
		case 8541:  *x=224;   *y=160; break;
		case 8542:  *x=232;   *y=160; break;
		case 8592:  *x=240;   *y=160; break;
		case 8593:  *x=248;   *y=160; break;
		case 8594:  *x=0;     *y=168; break;
		case 8595:  *x=8;     *y=168; break;
		case 8596:  *x=16;    *y=168; break;
		case 8597:  *x=24;    *y=168; break;
		case 8616:  *x=32;    *y=168; break;
		case 8706:  *x=40;    *y=168; break;
		case 8709:  *x=48;    *y=168; break;
		case 8710:  *x=56;    *y=168; break;
		case 8712:  *x=64;    *y=168; break;
		case 8719:  *x=72;    *y=168; break;
		case 8721:  *x=80;    *y=168; break;
		case 8722:  *x=88;    *y=168; break;
		case 8725:  *x=96;    *y=168; break;
		case 8729:  *x=104;   *y=168; break;
		case 8730:  *x=112;   *y=168; break;
		case 8734:  *x=120;   *y=168; break;
		case 8735:  *x=128;   *y=168; break;
		case 8745:  *x=136;   *y=168; break;
		case 8747:  *x=144;   *y=168; break;
		case 8776:  *x=152;   *y=168; break;
		case 8800:  *x=160;   *y=168; break;
		case 8801:  *x=168;   *y=168; break;
		case 8804:  *x=176;   *y=168; break;
		case 8805:  *x=184;   *y=168; break;
		case 8857:  *x=192;   *y=168; break;
		case 8960:  *x=200;   *y=168; break;
		case 8962:  *x=208;   *y=168; break;
		case 8976:  *x=216;   *y=168; break;
		case 8992:  *x=224;   *y=168; break;
		case 8993:  *x=232;   *y=168; break;
		case 9472:  *x=240;   *y=168; break;
		case 9474:  *x=248;   *y=168; break;
		case 9484:  *x=0;     *y=176; break;
		case 9488:  *x=8;     *y=176; break;
		case 9492:  *x=16;    *y=176; break;
		case 9496:  *x=24;    *y=176; break;
		case 9500:  *x=32;    *y=176; break;
		case 9508:  *x=40;    *y=176; break;
		case 9516:  *x=48;    *y=176; break;
		case 9524:  *x=56;    *y=176; break;
		case 9532:  *x=64;    *y=176; break;
		case 9552:  *x=72;    *y=176; break;
		case 9553:  *x=80;    *y=176; break;
		case 9554:  *x=88;    *y=176; break;
		case 9555:  *x=96;    *y=176; break;
		case 9556:  *x=104;   *y=176; break;
		case 9557:  *x=112;   *y=176; break;
		case 9558:  *x=120;   *y=176; break;
		case 9559:  *x=128;   *y=176; break;
		case 9560:  *x=136;   *y=176; break;
		case 9561:  *x=144;   *y=176; break;
		case 9562:  *x=152;   *y=176; break;
		case 9563:  *x=160;   *y=176; break;
		case 9564:  *x=168;   *y=176; break;
		case 9565:  *x=176;   *y=176; break;
		case 9566:  *x=184;   *y=176; break;
		case 9567:  *x=192;   *y=176; break;
		case 9568:  *x=200;   *y=176; break;
		case 9569:  *x=208;   *y=176; break;
		case 9570:  *x=216;   *y=176; break;
		case 9571:  *x=224;   *y=176; break;
		case 9572:  *x=232;   *y=176; break;
		case 9573:  *x=240;   *y=176; break;
		case 9574:  *x=248;   *y=176; break;
		case 9575:  *x=0;     *y=184; break;
		case 9576:  *x=8;     *y=184; break;
		case 9577:  *x=16;    *y=184; break;
		case 9578:  *x=24;    *y=184; break;
		case 9579:  *x=32;    *y=184; break;
		case 9580:  *x=40;    *y=184; break;
		case 9600:  *x=48;    *y=184; break;
		case 9601:  *x=56;    *y=184; break;
		case 9604:  *x=64;    *y=184; break;
		case 9608:  *x=72;    *y=184; break;
		case 9612:  *x=80;    *y=184; break;
		case 9616:  *x=88;    *y=184; break;
		case 9617:  *x=96;    *y=184; break;
		case 9618:  *x=104;   *y=184; break;
		case 9619:  *x=112;   *y=184; break;
		case 9632:  *x=120;   *y=184; break;
		case 9633:  *x=128;   *y=184; break;
		case 9642:  *x=136;   *y=184; break;
		case 9643:  *x=144;   *y=184; break;
		case 9644:  *x=152;   *y=184; break;
		case 9650:  *x=160;   *y=184; break;
		case 9658:  *x=168;   *y=184; break;
		case 9660:  *x=176;   *y=184; break;
		case 9668:  *x=184;   *y=184; break;
		case 9674:  *x=192;   *y=184; break;
		case 9675:  *x=200;   *y=184; break;
		case 9679:  *x=208;   *y=184; break;
		case 9688:  *x=216;   *y=184; break;
		case 9689:  *x=224;   *y=184; break;
		case 9702:  *x=232;   *y=184; break;
		case 9786:  *x=240;   *y=184; break;
		case 9787:  *x=248;   *y=184; break;
		case 9788:  *x=0;     *y=192; break;
		case 9792:  *x=8;     *y=192; break;
		case 9794:  *x=16;    *y=192; break;
		case 9824:  *x=24;    *y=192; break;
		case 9827:  *x=32;    *y=192; break;
		case 9829:  *x=40;    *y=192; break;
		case 9830:  *x=48;    *y=192; break;
		case 9834:  *x=56;    *y=192; break;
		case 9835:  *x=64;    *y=192; break;
		case 10003: *x=72;    *y=192; break;
		case 64257: *x=80;    *y=192; break;
		case 64258: *x=88;    *y=192; break;
		case 65533: *x=96;    *y=192; break;
		default: *x = 96; *y = 192; break;
	}
}

class olcSprite
{
public:
	olcSprite()
	{

	}

	olcSprite(int w, int h)
	{
		Create(w, h);
	}

	olcSprite(wstring sFile)
	{
		if (!Load(sFile))
			Create(8, 8);
	}

	int nWidth = 0;
	int nHeight = 0;

//Yer touching private things king!
//private:
	wchar_t *m_Glyphs = nullptr;
	short *m_Colours = nullptr;

private:
	void Create(int w, int h)
	{
		nWidth = w;
		nHeight = h;
		m_Glyphs = new wchar_t[w*h];
		m_Colours = new short[w*h];
		for (int i = 0; i < w*h; i++)
		{
			m_Glyphs[i] = L' ';
			m_Colours[i] = FG_BLACK;
		}
	}

public:
	void SetGlyph(int x, int y, wchar_t c)
	{
		if (x <0 || x >= nWidth || y < 0 || y >= nHeight)
			return;
		else
			m_Glyphs[y * nWidth + x] = c;
	}

	void SetColour(int x, int y, short c)
	{
		if (x <0 || x >= nWidth || y < 0 || y >= nHeight)
			return;
		else
			m_Colours[y * nWidth + x] = c;
	}

	wchar_t GetGlyph(int x, int y)
	{
		if (x <0 || x >= nWidth || y < 0 || y >= nHeight)
			return L' ';
		else
			return m_Glyphs[y * nWidth + x];
	}

	short GetColour(int x, int y)
	{
		if (x <0 || x >= nWidth || y < 0 || y >= nHeight)
			return FG_BLACK;
		else
			return m_Colours[y * nWidth + x];
	}

	wchar_t SampleGlyph(float x, float y)
	{
		int sx = (int)(x * (float)nWidth);
		int sy = (int)(y * (float)nHeight-1.0f);
		if (sx <0 || sx >= nWidth || sy < 0 || sy >= nHeight)
			return L' ';
		else
			return m_Glyphs[sy * nWidth + sx];
	}

	short SampleColour(float x, float y)
	{
		int sx = (int)(x * (float)nWidth);
		int sy = (int)(y * (float)nHeight-1.0f);
		if (sx <0 || sx >= nWidth || sy < 0 || sy >= nHeight)
			return FG_BLACK;
		else
			return m_Colours[sy * nWidth + sx];
	}

	bool Save(wstring sFile)
	{
		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");
		if (f == nullptr)
			return false;

		fwrite(&nWidth, sizeof(int), 1, f);
		fwrite(&nHeight, sizeof(int), 1, f);
		fwrite(m_Colours, sizeof(short), nWidth * nHeight, f);
		fwrite(m_Glyphs, sizeof(wchar_t), nWidth * nHeight, f);

		fclose(f);

		return true;
	}

	bool Load(wstring sFile)
	{
		delete[] m_Glyphs;
		delete[] m_Colours;
		nWidth = 0;
		nHeight = 0;

		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"rb");
		if (f == nullptr)
			return false;

		fread(&nWidth, sizeof(int), 1, f);
		fread(&nHeight, sizeof(int), 1, f);

		Create(nWidth, nHeight);

		fread(m_Colours, sizeof(short), nWidth * nHeight, f);
		fread(m_Glyphs, sizeof(wchar_t), nWidth * nHeight, f);

		fclose(f);
		return true;
	}

};

class olcConsoleGameEngine
{
	uint32_t m_ColourPalette[16] = // 0xAABBGGRR
	{
		0xFF000000, // BLACK
		0xFF800000, // DARK_BLUE
		0xFF008000, // DARK_GREEN
		0xFF808000, // DARK_CYAN
		0xFF000080, // DARK_RED
		0xFF800080, // DARK_MAGENTA
		0xFF008080, // DARK_YELLOW
		0xFFC0C0C0, // GREY
		0xFF808080, // DARK_GREY
		0xFFFF0000, // BLUE
		0xFF00FF00, // GREEN
		0xFFFFFF00, // CYAN
		0xFF0000FF, // RED
		0xFFFF00FF, // MAGENTA
		0xFF00FFFF, // YELLOW
		0xFFFFFFFF  // WHITE
	};
	
	void UpdateMousePosition(int x, int y)
	{
		float fx = (x - m_fDrawOffsetX) / (m_nScreenWidth * m_nFontWidth * m_fDrawScale);
		float fy = (y - m_fDrawOffsetY) / (m_nScreenHeight * m_nFontHeight * m_fDrawScale);
		
		fx = fx < 0 ? 0 : fx > 1.0f ? 1.0f : fx;
		fy = fy < 0 ? 0 : fy > 1.0f ? 1.0f : fy;
		
		m_mousePosX  = (int)(fx * m_nScreenWidth);
		m_mousePosY  = (int)(fy * m_nScreenHeight);
	}
	
	void ToggleFullscreen(HWND hWnd)
	{
		static WINDOWPLACEMENT prev = {sizeof(WINDOWPLACEMENT)};
		
		DWORD style = GetWindowLong(hWnd, GWL_STYLE);
		
		if(style & WS_OVERLAPPEDWINDOW)
		{
			int width  = GetSystemMetrics(SM_CXSCREEN);
			int height = GetSystemMetrics(SM_CYSCREEN);
			
			if(GetWindowPlacement(hWnd, &prev))
			{
				SetWindowLong(hWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			}
		}
		else
		{
			SetWindowLong(hWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
			SetWindowPlacement(hWnd, &prev);
			SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}	
	}

	void WindowResize(void)
	{
		DWORD style  = GetWindowLong(m_hWnd, GWL_STYLE);
		DWORD stylex = GetWindowLong(m_hWnd, GWL_EXSTYLE);
		
		RECT rWndRect = { 0, 0, m_nWindowWidth, m_nWindowHeight };

		if (style & WS_OVERLAPPEDWINDOW)
		{
			AdjustWindowRectEx(&rWndRect, style, FALSE, stylex);

			int width = rWndRect.right - rWndRect.left;
			int height = rWndRect.bottom - rWndRect.top;

			SetWindowPos(m_hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER);
		}
		else
		{
			ToggleFullscreen(m_hWnd);

			AdjustWindowRectEx(&rWndRect, style, FALSE, stylex);

			int width = rWndRect.right - rWndRect.left;
			int height = rWndRect.bottom - rWndRect.top;

			SetWindowPos(m_hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER);

			ToggleFullscreen(m_hWnd);
		}
		
	}

	void WindowUpdateScale(void)
	{
		int width = m_nWindowWidth;
		int height = m_nWindowHeight;
		
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, -1, 100);
		glMatrixMode(GL_MODELVIEW);
		
		float scaleX = (float)width / (float)(m_nScreenWidth * m_nFontWidth);
		float scaleY = (float)height / (float)(m_nScreenHeight * m_nFontHeight);
		
		if(scaleX < scaleY)
		{
			m_fDrawScale = scaleX;
			m_fDrawOffsetX = 0;
			m_fDrawOffsetY = ((float)height - (float)(m_nScreenHeight * m_nFontHeight * scaleX)) * 0.5f;
		}
		else
		{
			m_fDrawScale = scaleY;
			m_fDrawOffsetX = ((float)width - (float)(m_nScreenWidth * m_nFontWidth * scaleY)) * 0.5f;
			m_fDrawOffsetY = 0;
		}
	}

	int SetPixelFormatGL(void)
	{
		PIXELFORMATDESCRIPTOR pfd = 
		{
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		int pf = ChoosePixelFormat(m_hDevCtx, &pfd);
		if(!pf) return 0;
		
		return SetPixelFormat(m_hDevCtx, pf, &pfd);
	}
	
	static
	LRESULT CALLBACK olcWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static olcConsoleGameEngine *cge;
		
		
		switch(uMsg)
		{
			case WM_CREATE:
				cge = static_cast<olcConsoleGameEngine*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
			
				cge->m_hDevCtx = GetDC(hWnd);
				if(!cge->SetPixelFormatGL()) return -1;

				cge->m_hRenCtx = wglCreateContext(cge->m_hDevCtx);
				if(!cge->m_hRenCtx) return -1;
				
				wglMakeCurrent(cge->m_hDevCtx, cge->m_hRenCtx);
				ShowWindow(cge->m_hConsole, SW_HIDE);
				return 0;
				
			case WM_SYSCHAR:
				//ding ding ding
				return 0;
				
			case WM_MOUSEMOVE:
				cge->UpdateMousePosition(LOWORD(lParam), HIWORD(lParam));
				return 0;
				
			case WM_SIZE:
				cge->m_nWindowWidth = LOWORD(lParam);
				cge->m_nWindowHeight = HIWORD(lParam);
				cge->m_bDoWindowUpdate = true;
				return 0;
			
			case WM_SETFOCUS:
				cge->m_bConsoleInFocus = true;
				return 0;
				
			case WM_KILLFOCUS:
				cge->m_bConsoleInFocus = false;
				return 0;
				
			case WM_CLOSE:
				m_bAtomActive = false;
				return 0;
				
			case WM_DESTROY:
				ShowWindow(cge->m_hConsole, SW_SHOW);
				PostQuitMessage(0);
				return 0;

			case 0x8000:
				cge->ToggleFullscreen(hWnd);
				return 0;

			case 0x8001:
				cge->WindowResize();
				return 0;
		}
		
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	HWND ConstructWindow(int width, int height)
	{
		wchar_t wnd_title[] = L"OneLoneCoder.com - Console Game Engine (OGL)";
		wchar_t wnd_class[] = L"OLC_CONSOLE_GAME_ENGINE_CLASS";
	
		HINSTANCE hInstance = GetModuleHandle(NULL);
	
		WNDCLASS wc =
		{
			CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			olcWndProc,
			0,
			0,
			hInstance, 
			LoadIcon(NULL, IDI_APPLICATION),
			LoadCursor(NULL, IDC_ARROW),
			NULL,
			NULL,
			wnd_class
		};
		
		RegisterClass(&wc);
		
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES;
		DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		RECT rWndRect = { 0, 0, width, height };
		
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);

		width  = rWndRect.right - rWndRect.left;
		height = rWndRect.bottom - rWndRect.top;

		return CreateWindowEx(dwExStyle, wnd_class, wnd_title, dwStyle,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, this);
	}

public:
	olcConsoleGameEngine()
	{
		m_nScreenWidth = 80;
		m_nScreenHeight = 30;

		memset(m_keyNewState, 0, 256 * sizeof(short));
		memset(m_keyOldState, 0, 256 * sizeof(short));
		memset(m_keys, 0, 256 * sizeof(sKeyState));
		m_mousePosX = 0;
		m_mousePosY = 0;

		m_sAppName = L"Default";

		//grab 1 GB or memory
		m_bufMemory = (uint8_t*)VirtualAlloc(NULL, 1024 * 1024 * 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!m_bufMemory) throw exception("No Memory!");

		m_bufScreen = (CHAR_INFO*)&m_bufMemory[0];
		m_bufScreen_old = (CHAR_INFO*)&m_bufMemory[21474304];

		m_fVertexArray = (float*)&m_bufMemory[42948608];
		m_fTexCoordArray = (float*)&m_bufMemory[300640256];
		m_uIndicesArray = (uint32_t*)&m_bufMemory[558331904];

		m_uForegroundColorArray = (uint32_t*)&m_bufMemory[816023552];
		m_uBackgroundColorArray = (uint32_t*)&m_bufMemory[944869376];
		
		m_hConsole = GetConsoleWindow();
	}

	int ConstructConsole(int width, int height, int fontw, int fonth)
	{
		m_nScreenWidth = width;
		m_nScreenHeight = height;

		m_nFontWidth = fontw;
		m_nFontHeight = fonth;

		int newWndWidth = width * fontw;
		int newWndHeight = height * fonth;

		if (m_hWnd && ((m_nWindowWidth != newWndWidth) || (m_nWindowHeight != newWndHeight)))
		{
			SendMessage(m_hWnd, 0x8001, 0, 0);
		}

		m_nWindowWidth = newWndWidth;
		m_nWindowHeight = newWndHeight;

		// Allocate memory for screen buffer
		size_t bufLen = m_nScreenWidth * m_nScreenHeight;

		if (bufLen > 0x51EB00)
		{
			MessageBoxA(NULL, "Not enough memory!", "ERROR!", MB_OK);
			ExitProcess(0xDEADC0DE);
		}

		memset(m_bufMemory, 0, bufLen * 200);

		for(int y = 0; y < m_nScreenHeight; y++)
		for(int x = 0; x < m_nScreenWidth; x++)
		{
			int pos = y * m_nScreenWidth + x;
			
			float x1 = (float)(x);
			float y1 = (float)(y);
			float x2 = (float)(x+1);
			float y2 = (float)(y+1);
		
			pos *= 12;

			m_fVertexArray[pos + 0]  = x1;
			m_fVertexArray[pos + 1]  = y1;
			m_fVertexArray[pos + 2]  = x2;
			m_fVertexArray[pos + 3]  = y1;
			m_fVertexArray[pos + 4]  = x1;
			m_fVertexArray[pos + 5]  = y2;
			m_fVertexArray[pos + 6]  = x2;
			m_fVertexArray[pos + 7]  = y1;
			m_fVertexArray[pos + 8]  = x1;
			m_fVertexArray[pos + 9]  = y2;
			m_fVertexArray[pos + 10] = x2;
			m_fVertexArray[pos + 11] = y2;
			
			m_uIndicesArray[pos + 0]  = pos + 0;
			m_uIndicesArray[pos + 1]  = pos + 1;
			m_uIndicesArray[pos + 2]  = pos + 2;
			m_uIndicesArray[pos + 3]  = pos + 3;
			m_uIndicesArray[pos + 4]  = pos + 4;
			m_uIndicesArray[pos + 5]  = pos + 5;
			m_uIndicesArray[pos + 6]  = pos + 6;
			m_uIndicesArray[pos + 7]  = pos + 7;
			m_uIndicesArray[pos + 8]  = pos + 8;
			m_uIndicesArray[pos + 9]  = pos + 9;
			m_uIndicesArray[pos + 10] = pos + 10;
			m_uIndicesArray[pos + 11] = pos + 11;
		}

		return 1;
	}
	
	virtual void Draw(int x, int y, wchar_t c = 0x2588, short col = 0x000F)
	{
		if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
		{
			m_bufScreen[y * m_nScreenWidth + x].Char.UnicodeChar = c;
			m_bufScreen[y * m_nScreenWidth + x].Attributes = col;
		}
	}

	void Fill(int x1, int y1, int x2, int y2, wchar_t c = 0x2588, short col = 0x000F)
	{
		Clip(x1, y1);
		Clip(x2, y2);
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}

	void DrawString(int x, int y, wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			m_bufScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = c[i];
			m_bufScreen[y * m_nScreenWidth + x + i].Attributes = col;
		}
	}

	void DrawStringAlpha(int x, int y, wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			if (c[i] != L' ')
			{
				m_bufScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = c[i];
				m_bufScreen[y * m_nScreenWidth + x + i].Attributes = col;
			}
		}
	}

	void Clip(int &x, int &y)
	{
		if (x < 0) x = 0;
		if (x >= m_nScreenWidth) x = m_nScreenWidth;
		if (y < 0) y = 0;
		if (y >= m_nScreenHeight) y = m_nScreenHeight;
	}

	void DrawLine(int x1, int y1, int x2, int y2, wchar_t c = 0x2588, short col = 0x000F)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1;
		dy = y2 - y1;
		dx1 = abs(dx);
		dy1 = abs(dy);
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}
			Draw(x, y, c, col);
			for (i = 0; x<xe; i++)
			{
				x = x + 1;
				if (px<0)
					px = px + 2 * dy1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0))
						y = y + 1;
					else
						y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw(x, y, c, col);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}
			Draw(x, y, c, col);
			for (i = 0; y<ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0))
						x = x + 1;
					else
						x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw(x, y, c, col);
			}
		}
	}

	void DrawCircle(int xc, int yc, int r, wchar_t c = 0x2588, short col = 0x000F)
	{
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		while (y >= x) // only formulate 1/8 of circle
		{
			Draw(xc - x, yc - y, c, col);//upper left left
			Draw(xc - y, yc - x, c, col);//upper upper left
			Draw(xc + y, yc - x, c, col);//upper upper right
			Draw(xc + x, yc - y, c, col);//upper right right
			Draw(xc - x, yc + y, c, col);//lower left left
			Draw(xc - y, yc + x, c, col);//lower lower left
			Draw(xc + y, yc + x, c, col);//lower lower right
			Draw(xc + x, yc + y, c, col);//lower right right
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	}

	void FillCircle(int xc, int yc, int r, wchar_t c = 0x2588, short col = 0x000F)
	{
		// Taken from wikipedia
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		auto drawline = [&](int sx, int ex, int ny)
		{
			for (int i = sx; i <= ex; i++)
				Draw(i, ny, c, col);
		};

		while (y >= x)
		{
			// Modified to draw scan-lines instead of edges
			drawline(xc - x, xc + x, yc - y);
			drawline(xc - y, xc + y, yc - x);
			drawline(xc - x, xc + x, yc + y);
			drawline(xc - y, xc + y, yc + x);
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	};

	void DrawSprite(int x, int y, olcSprite *sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->nWidth; i++)
		{
			for (int j = 0; j < sprite->nHeight; j++)
			{
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j));
			}
		}
	}

	void DrawPartialSprite(int x, int y, olcSprite *sprite, int ox, int oy, int w, int h)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				if (sprite->GetGlyph(i+ox, j+oy) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i+ox, j+oy), sprite->GetColour(i+ox, j+oy));
			}
		}
	}

	void DrawWireFrameModel(const vector<pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		vector<pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
				(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, PIXEL_SOLID, col);
		}
	}
	
	~olcConsoleGameEngine()
	{
		if (m_bufMemory) VirtualFree(m_bufMemory, 0, MEM_RELEASE);

		m_bufMemory = nullptr;

		m_bufScreen = nullptr;
		m_bufScreen_old = nullptr;

		m_fVertexArray = nullptr;
		m_fTexCoordArray = nullptr;
		m_uIndicesArray = nullptr;

		m_uForegroundColorArray = nullptr;
		m_uBackgroundColorArray = nullptr;
	}
	
	void GenerateMipmapPow2(uint8_t *tex_new, uint8_t *tex_old, uint8_t *ref_alpha, int size)
	{
		for(int y = 0; y < size; y++)
		for(int x = 0; x < size; x++)
		{
			uint8_t *p0 = &tex_old[(y<<1) * (size<<1) + (x<<1)];
			uint8_t *p1 = &p0[(size<<1)];
			
			tex_new[y * size + x] = (p0[0] + p0[1] + p1[0] + p1[1]) >> 2;
		}
		
		int char_size = size >> 5;
		
		for(int i = 0; i < 1024; i++)
		{
			int alpha = 0;
			
			int posy = (i >> 5) * char_size;
			int posx = (i & 0x1F) * char_size;
			
			for(int y = 0; y < char_size; y++)
			for(int x = 0; x < char_size; x++)
			{
				alpha += tex_new[(posy + y) * size + (posx + x)];
			}
			
			alpha /= char_size * char_size;
			
			float factor = (float)ref_alpha[i] / (float)alpha;
			
			for(int y = 0; y < char_size; y++)
			for(int x = 0; x < char_size; x++)
			{
				int value = tex_new[(posy + y) * size + (posx + x)] * factor;
				tex_new[(posy + y) * size + (posx + x)] = value > 255 ? 255 : value;
			}
		}
		
	}

public:
	void Start()
	{
		m_bAtomActive = true;
		
		m_hWnd = ConstructWindow(m_nWindowWidth, m_nWindowHeight);
		if(!m_hWnd)
		{
			Error(L"Could not create GL window");
			return;
		}
		
		glGenTextures(1, &m_uFontTexture);
		glBindTexture(GL_TEXTURE_2D, m_uFontTexture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	
		//mipmap generation
		{
			font_decode_custom_base64(); // fill pxplus_ibm_cga
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pxplus_ibm_cga);
			
			uint8_t *glyph_alpha = new uint8_t[1024]; // 32 * 32

			for(int i = 0; i < 1024; i++)
			{
				int alpha = 0;
				
				int posy = (i >> 5) << 3;
				int posx = (i & 0x1F) << 3;
				
				for(int y = 0; y < 8; y++)
				for(int x = 0; x < 8; x++)
				{
					alpha += pxplus_ibm_cga[(posy + y) * 256 + (posx + x)];
				}
				
				glyph_alpha[i] = (uint8_t)(alpha >> 6);
			}
		
			int texsize = 128;
			uint8_t *texbuf = new uint8_t[texsize * texsize];
			
			GenerateMipmapPow2(texbuf, pxplus_ibm_cga, glyph_alpha, texsize);
			glTexImage2D(GL_TEXTURE_2D, 1, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);

			texsize = 64;
			GenerateMipmapPow2(texbuf, texbuf, glyph_alpha, texsize);
			glTexImage2D(GL_TEXTURE_2D, 2, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);
			
			texsize = 32;
			GenerateMipmapPow2(texbuf, texbuf, glyph_alpha, texsize);
			glTexImage2D(GL_TEXTURE_2D, 3, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);
			
			texsize = 16;
			for(int level = 4; level < 9; level++)
			{
				for(int y = 0; y < texsize; y++)
				for(int x = 0; x < texsize; x++)
				{
					uint8_t *p0 = &texbuf[(y<<1) * (texsize<<1) + (x<<1)];
					uint8_t *p1 = &p0[texsize<<1];

					texbuf[y * texsize + x] = (p0[0] + p0[1] + p1[0] + p1[1]) >> 2;
				}
				
				glTexImage2D(GL_TEXTURE_2D, level, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);
				texsize >>= 1;
			}
			
			texsize = 1;
			for(int level = 9; level < 1000; level++)
			{
				glTexImage2D(GL_TEXTURE_2D, level, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);
			}
			
			delete[] texbuf;
			delete[] glyph_alpha;
		}

		wglSwapInterval = (wglSwapInterval_t*)wglGetProcAddress("wglSwapIntervalEXT");
		wglSwapInterval(0);
		
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		wglMakeCurrent(NULL, NULL);

		// Star the thread
		thread t = thread(&olcConsoleGameEngine::GameThread, this);
		
		MSG msg;
		while(GetMessage(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		// Wait for thread to be exited
		t.join();
	}

	int ScreenWidth()
	{
		return m_nScreenWidth;
	}

	int ScreenHeight() 
	{
		return m_nScreenHeight;
	}

private:
	void GameThread()
	{
		wglMakeCurrent(m_hDevCtx, m_hRenCtx);
		
		// Create user resources as part of this thread
		if (!OnUserCreate())
			m_bAtomActive = false;

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		
		glVertexPointer(2, GL_FLOAT, 0, m_fVertexArray);
		glTexCoordPointer(2, GL_FLOAT, 0, m_fTexCoordArray);
		
		LARGE_INTEGER timeFreq, timeNew, timeOld;
		QueryPerformanceFrequency(&timeFreq);
		QueryPerformanceCounter(&timeOld);
		QueryPerformanceCounter(&timeNew);
		
		int nFrameCounter = 0;
		float fFrameTimeAccum = 0;
		
		while (m_bAtomActive)
		{
			// Run as fast as possible
			while (m_bAtomActive)
			{			
				QueryPerformanceCounter(&timeNew);
				float fElapsedTime = (float)((timeNew.QuadPart - timeOld.QuadPart) / (double)timeFreq.QuadPart);
				timeOld = timeNew;

				for (int i = 0; i < 256; i++)
				{
					m_keyNewState[i] = GetAsyncKeyState(i) >> 15;
					
					m_keys[i].bPressed = false;
					m_keys[i].bReleased = false;

					if (m_keyNewState[i] != m_keyOldState[i])
					{
						if (m_keyNewState[i])
						{
							m_keys[i].bPressed = true;
							m_keys[i].bHeld = true;
						}
						else
						{
							m_keys[i].bReleased = true;
							m_keys[i].bHeld = false;
						}
					}

					m_keyOldState[i] = m_keyNewState[i];
				}
				
				m_mouse[0x00] = m_keys[VK_LBUTTON];
				m_mouse[0x01] = m_keys[VK_RBUTTON];
				m_mouse[0x02] = m_keys[VK_MBUTTON];
				m_mouse[0x03] = m_keys[0x05]; // VK_XBUTTON1
				m_mouse[0x04] = m_keys[0x06]; // VK_XBUTTON2

				if(m_keys[VK_MENU].bHeld && m_keys[VK_RETURN].bPressed)
				{
					SendMessage(m_hWnd, 0x8000, 0, 0);
				}
				
				if(m_bDoWindowUpdate)
				{
					WindowUpdateScale();
					m_bDoWindowUpdate = false;
				}
				
				glClear(GL_COLOR_BUFFER_BIT);
		
				// Handle Frame Update
				if (!OnUserUpdate(fElapsedTime))
				{
					m_bAtomActive = false;
					break;
				}

				// draw the things
				glPushMatrix();
				glTranslatef(m_fDrawOffsetX, m_fDrawOffsetY, 0.0f);
				glScalef(m_fDrawScale * m_nFontWidth, m_fDrawScale * m_nFontHeight, 1.0f);

				for(int y = 0; y < m_nScreenHeight; y++)
				for(int x = 0; x < m_nScreenWidth; x++)
				{
					int pos = y * m_nScreenWidth + x;
					
					if((m_bufScreen[pos].Char.UnicodeChar == m_bufScreen_old[pos].Char.UnicodeChar) &&
					   (m_bufScreen[pos].Attributes == m_bufScreen_old[pos].Attributes))
							continue;
							
					m_bufScreen_old[pos] = m_bufScreen[pos];

					WCHAR id = m_bufScreen[pos].Char.UnicodeChar;
					WORD col = m_bufScreen[pos].Attributes;
				
					int u, v;
					float u1, v1, u2, v2;
					uint32_t fg, bg;
					
					if(id == L' ')
					{
						u1 = u2 = v1 = v2 = 0.0f;
						fg = bg = 0;
					}
					else
					{
						GetFontCoords(id, &u, &v);
						
						u1 = (u)/256.0f;
						v1 = (v)/256.0f;
						u2 = (u + 8)/256.0f;
						v2 = (v + 8)/256.0f;
						
						fg = m_ColourPalette[col&0xF];
						bg = m_ColourPalette[(col>>4)&0xF];
					}
					
					pos *= 6;

					m_uForegroundColorArray[pos + 0]  = fg;
					m_uForegroundColorArray[pos + 1]  = fg;
					m_uForegroundColorArray[pos + 2]  = fg;
					m_uForegroundColorArray[pos + 3]  = fg;
					m_uForegroundColorArray[pos + 4]  = fg;
					m_uForegroundColorArray[pos + 5]  = fg;
					
					m_uBackgroundColorArray[pos + 0]  = bg;
					m_uBackgroundColorArray[pos + 1]  = bg;
					m_uBackgroundColorArray[pos + 2]  = bg;
					m_uBackgroundColorArray[pos + 3]  = bg;
					m_uBackgroundColorArray[pos + 4]  = bg;
					m_uBackgroundColorArray[pos + 5]  = bg;

					pos *= 2;

					m_fTexCoordArray[pos + 0]  = u1;
					m_fTexCoordArray[pos + 1]  = v1;
					m_fTexCoordArray[pos + 2]  = u2;
					m_fTexCoordArray[pos + 3]  = v1;
					m_fTexCoordArray[pos + 4]  = u1;
					m_fTexCoordArray[pos + 5]  = v2;
					m_fTexCoordArray[pos + 6]  = u2;
					m_fTexCoordArray[pos + 7]  = v1;
					m_fTexCoordArray[pos + 8]  = u1;
					m_fTexCoordArray[pos + 9]  = v2;
					m_fTexCoordArray[pos + 10] = u2;
					m_fTexCoordArray[pos + 11] = v2;					
				}
				
				glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_uBackgroundColorArray);
				glDrawArrays(GL_TRIANGLES, 0, m_nScreenWidth * m_nScreenHeight * 6);
				//glDrawElements(GL_TRIANGLES, m_nScreenWidth * m_nScreenHeight * 6, GL_UNSIGNED_INT, m_uIndicesArray);
				
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				
				glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_uForegroundColorArray);
				glDrawArrays(GL_TRIANGLES, 0, m_nScreenWidth * m_nScreenHeight * 6);
				
				glDisable(GL_BLEND);
				glDisable(GL_TEXTURE_2D);

				glPopMatrix();

				// Update Title & Present Screen Buffer
				wchar_t sNewTitle[256];
				swprintf_s(sNewTitle, 256, L"OneLoneCoder.com - Console Game Engine (OGL) - %s - FPS: %3.2f", m_sAppName.c_str(), 1.0f / fElapsedTime);
				SetWindowText(m_hWnd, sNewTitle);
				
				SwapBuffers(m_hDevCtx);
			}

			if (OnUserDestroy())
			{
				// User has permitted destroy, so exit and clean up
				m_cvGameFinished.notify_one();
			}
			else
			{
				// User denied destroy for some reason, so continue running
				m_bAtomActive = true;
			}
		}
		
		PostMessage(m_hWnd, WM_DESTROY, 0, 0);
	}

public:
	// User MUST OVERRIDE THESE!!
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fElapsedTime) = 0;	

	// Optional for clean up 
	virtual bool OnUserDestroy()
	{
		return true;
	}


protected:
	

	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[256], m_mouse[5];

	int m_mousePosX;
	int m_mousePosY;

public:
	sKeyState GetKey(int nKeyID){ return m_keys[nKeyID]; }
	int GetMouseX() { return m_mousePosX; }
	int GetMouseY() { return m_mousePosY; }
	sKeyState GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
	bool IsFocused() { return m_bConsoleInFocus; }


protected:
	int Error(const wchar_t *msg)
	{
		wchar_t buff1[256];
		wchar_t buff2[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, buff1, 256, NULL);
		wsprintf(buff2, L"%s\n\n%s", msg, buff1);
		MessageBox(NULL, buff2, L"ERROR", MB_ICONERROR | MB_OK);
		return 0;
	}

protected:
	int m_nScreenWidth;
	int m_nScreenHeight;
	int m_nWindowWidth;
	int m_nWindowHeight;
	int m_nFontWidth;
	int m_nFontHeight;
	float m_fDrawScale;
	float m_fDrawOffsetX;
	float m_fDrawOffsetY;
	float *m_fVertexArray;
	uint32_t *m_uIndicesArray;
	uint32_t *m_uForegroundColorArray;
	uint32_t *m_uBackgroundColorArray;
	float *m_fTexCoordArray;
	CHAR_INFO *m_bufScreen;
	CHAR_INFO *m_bufScreen_old;
	uint8_t *m_bufMemory;
	wstring m_sAppName;
	SMALL_RECT m_rectWindow;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };
	bool m_mouseOldState[5] = { 0 };
	bool m_mouseNewState[5] = { 0 };
	bool m_bConsoleInFocus = true;
	bool m_bDoWindowUpdate = false;
	HWND  m_hConsole = nullptr;
	HWND  m_hWnd     = nullptr;
	HDC   m_hDevCtx  = nullptr;
	HGLRC m_hRenCtx  = nullptr;
	GLuint m_uFontTexture;
	static atomic<bool> m_bAtomActive;
	static condition_variable m_cvGameFinished;
	static mutex m_muxGame;
};

atomic<bool> olcConsoleGameEngine::m_bAtomActive(false);
condition_variable olcConsoleGameEngine::m_cvGameFinished;
mutex olcConsoleGameEngine::m_muxGame;
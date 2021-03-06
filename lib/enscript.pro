%
% PostScript prolog.
% Copyright (c) 1995 Markku Rossi.
%
% Author: Markku Rossi <mtr@iki.fi>
%
%
% This file is part of GNU enscript.
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2, or (at your option)
% any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; see the file COPYING.  If not, write to
% the Free Software Foundation, 59 Temple Place - Suite 330,
% Boston, MA 02111-1307, USA.
%

% -- code follows this line --
%
% Procedures.
%

/_S {	% save current state
  /_s save def
} def
/_R {	% restore from saved state
  _s restore
} def

/S {	% showpage protecting gstate
  gsave
  showpage
  grestore
} bind def

/MF {	% fontname newfontname -> -	make a new encoded font
  /newfontname exch def
  /fontname exch def

  /fontdict fontname findfont def
  /newfont fontdict maxlength dict def

  fontdict {
    exch 
    dup /FID eq {
      % skip FID pair
      pop pop
    } {
      % copy to the new font dictionary
      exch newfont 3 1 roll put
    } ifelse
  } forall
  
  newfont /FontName newfontname put

  % insert only valid encoding vectors
  encoding_vector length 256 eq {
    newfont /Encoding encoding_vector put
  } if
  
  newfontname newfont definefont pop
} def

/M {moveto} bind def
/s {show} bind def

/Box {	% x y w h -> -			define box path
  /d_h exch def /d_w exch def /d_y exch def /d_x exch def
  d_x d_y  moveto 
  d_w 0 rlineto 
  0 d_h rlineto 
  d_w neg 0 rlineto 
  closepath
} def

% Highlight bars.
/highlight_bars {	% nlines lineheight output_y_margin gray -> -
  gsave
    setgray
    /ymarg exch def
    /lineheight exch def
    /nlines exch def

    % This 2 is just a magic number to sync highlight lines to text.
    0 d_header_y ymarg sub 2 sub translate
    
    /cw d_output_w cols div def
    /nrows d_output_h ymarg 2 mul sub lineheight div cvi def

    % for each column
    0 1 cols 1 sub {
      cw mul /xp exch def

      % for each rows
      0 1 nrows 1 sub {
        /rn exch def 
        rn lineheight mul neg /yp exch def
        rn nlines idiv 2 mod 0 eq {
	  % Draw highlight bar.  4 is just a magic indentation.
	  xp 4 add yp cw 8 sub lineheight neg Box fill	  
	} if
      } for
    } for
    
  grestore
} def

% Line highlight bar.
/line_highlight {	% x y width height gray -> -
  gsave
    /gray exch def
    Box gray setgray fill
  grestore
} def

% Column separator lines.
/column_lines {
  gsave
    .1 setlinewidth
    0 d_footer_h translate
    /cw d_output_w cols div def
    1 1 cols 1 sub {
      cw mul 0 moveto
      0 d_output_h rlineto stroke
    } for
  grestore
} def

% Column borders.
/column_borders {
  gsave
    .1 setlinewidth
    0 d_footer_h moveto 
    0 d_output_h rlineto
    d_output_w 0 rlineto
    0 d_output_h neg rlineto
    closepath stroke
  grestore
} def

% Do the actual underlay drawing
/draw_underlay {
  ul_style 0 eq {
    ul_str true charpath stroke
  } { 
    ul_str show
  } ifelse
} def

% Underlay
/underlay {	% - -> -
  gsave
    0 d_page_h translate
    d_page_h neg d_page_w atan rotate

    ul_gray setgray
    ul_font setfont
    /dw d_page_h dup mul d_page_w dup mul add sqrt def
    ul_str stringwidth pop dw exch sub 2 div ul_h_ptsize -2 div moveto 
    draw_underlay
  grestore
} def

/user_underlay {	% - -> -
  gsave
    ul_x ul_y translate
    ul_angle rotate
    ul_gray setgray
    ul_font setfont 
    0 0 ul_h_ptsize 2 div sub moveto 
    draw_underlay
  grestore
} def

% Page prefeed
/page_prefeed {		% bool -> -
  statusdict /prefeed known {
    statusdict exch /prefeed exch put
  } {
    pop
  } ifelse
} def

% EPSF import.

/BeginEPSF {
  /b4_Inc_state save def    		% Save state for cleanup
  /dict_count countdictstack def	% Count objects on dict stack
  /op_count count 1 sub def		% Count objects on operand stack 
  userdict begin
  /showpage { } def
  0 setgray 0 setlinecap
  1 setlinewidth 0 setlinejoin
  10 setmiterlimit [ ] 0 setdash newpath
  /languagelevel where {
    pop languagelevel
    1 ne {
      false setstrokeadjust false setoverprint 
    } if
  } if
} bind def

/EndEPSF {
  count op_count sub { pos } repeat	% Clean up stacks
  countdictstack dict_count sub { end } repeat
  b4_Inc_state restore
} bind def

% Check PostScript language level.
/languagelevel where {
  pop /gs_languagelevel languagelevel def
} {
  /gs_languagelevel 1 def
} ifelse

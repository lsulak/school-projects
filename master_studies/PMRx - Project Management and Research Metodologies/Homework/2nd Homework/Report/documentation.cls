% Ladislav Sulak
% vim: set syntax=tex encoding=latin2:
% ----------------------------------------------------------------------------

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{documentation}

%Default values of params
\newif\ifcover \let\ifcover\iffalse
\newif\ifPrint \let\ifPrint\iffalse
\newif\ifWis   \let\ifWis\iftrue

%Paramets of class

\DeclareOption{cover}{\let\ifcover\iftrue}
\DeclareOption{print}{\let\ifWis\iffalse \let\ifPrint\iftrue}

\DeclareOption*{}
\ProcessOptions\relax

%Default will be class REPORT
\ifx\pdfoutput\undefined % nejedeme pod pdftexem
   \LoadClass[a4paper,oneside,onecolumn,12pt]{report}
\else
   \LoadClass[pdftex,a4paper,oneside,onecolumn,12pt]{report}
\fi

%Packages
\usepackage{amsmath}
\usepackage{amssymb}
\usepackage[english]{babel}
\usepackage[latin2]{inputenc}
\main@language{english}
\ifx\pdfoutput\undefined
  \usepackage{graphics}
  \usepackage{epsfig}
\else
  \usepackage[pdftex]{graphicx}
\fi

%Sazebni obrazec
\setlength{\hoffset}{-0.7cm}
\setlength{\voffset}{-1.5cm}
\setlength{\textheight}{23.0cm}
\setlength{\textwidth}{15.2cm}

\def\@institute@EN{Institute}
\def\@author@EN{Author}
\def\@email@EN{Email}

\newif\ifnasel
\def\ifEqString#1#2#3#4{\def\testa{#1}\def\testb{#2}%
  \ifx\testa\testb \def#4{#3} \naseltrue \fi%
}

%Project type
\newcommand{\set@project}[1]{%
 \ifEqString{#1}{OP}{Technical work}{\@projecttype}%
 \ifEqString{#1}{SP}{Term project}{\@projecttype}%
}

%Name, surname and tituls of author
\def\get@author{\ifx\undefined\@author@tp\else%
\@author@tp~\fi{\@author}\ifx\undefined\@author@ta\else,~\@author@ta\fi}


%Params 
\define@key{projinfo}{title}{\global\def\@title{#1}}
\define@key{projinfo}{subject}{\global\def\@subject{#1}}
\define@key{projinfo}{project}{\set@project{#1}}
\define@key{projinfo}{month}{\def\@month{#1}}
\define@key{projinfo}{year}{\def\@year{#1}}
\define@key{projinfo}{author}{\def\@author{#1}}
\define@key{projinfo}{email}{\def\@email{#1}}
\define@key{projinfo}{institute}{\def\@institute{#1}}
\def\projectinfo{\setkeys{projinfo}}

\newlength{\mkspc}
\setlength{\mkspc}{2pt}

\def\setfontsize#1{\renewcommand{\baselinestretch}{1.5}\fontsize{#1}{\f@baselineskip}\selectfont}
\newcommand{\mktxt}[2]{\vbox{\setfontsize{#1}\noindent#2}\vspace\mkspc}
\newcommand{\mktxt@b}[2]{\vbox{\setfontsize{#1}\noindent\bf#2}\vspace\mkspc}
\newcommand{\mkuctxt}[2]{\vbox{\setfontsize{#1}\noindent{#2}}\vspace\mkspc}

%Title page \maketitle
\renewcommand\maketitle{%
\thispagestyle{empty}\null%

\vfill

\mkuctxt{18}{\@title}
\mkuctxt{12}{\@subject}

\vfill
\vspace{5mm}
\mktxt{14}{{\@author@EN} \hfill \get@author}

\vspace{5mm}
\mktxt{14}{{\@email@EN} \hfill \@email}

\vspace{5mm}
\mktxt{14}{\@institute@EN \hfill \@institute}

\vspace{5mm}
\mktxt{12}{Heraklion \@month/\@year}

\newpage\fontfamily{\familydefault}\selectfont%

}

\endinput

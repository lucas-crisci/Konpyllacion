max	identificateur	max
(	symbole	PARENTHESE_OUVRANTE
entier	mot_clef	entier
$a	identificateur	$a
,	symbole	VIRGULE
entier	mot_clef	entier
$b	identificateur	$b
)	symbole	PARENTHESE_FERMANTE
{	symbole	ACCOLADE_OUVRANTE
si	mot_clef	si
$a	identificateur	$a
<	symbole	INFERIEUR
$b	identificateur	$b
alors	mot_clef	alors
{	symbole	ACCOLADE_OUVRANTE
retour	mot_clef	retour
$b	identificateur	$b
;	symbole	POINT_VIRGULE
}	symbole	ACCOLADE_FERMANTE
retour	mot_clef	retour
$a	identificateur	$a
;	symbole	POINT_VIRGULE
}	symbole	ACCOLADE_FERMANTE
main	identificateur	main
(	symbole	PARENTHESE_OUVRANTE
)	symbole	PARENTHESE_FERMANTE
entier	mot_clef	entier
$v_1	identificateur	$v_1
,	symbole	VIRGULE
entier	mot_clef	entier
$v_2	identificateur	$v_2
;	symbole	POINT_VIRGULE
{	symbole	ACCOLADE_OUVRANTE
$v_1	identificateur	$v_1
=	symbole	EGAL
lire	mot_clef	lire
(	symbole	PARENTHESE_OUVRANTE
)	symbole	PARENTHESE_FERMANTE
;	symbole	POINT_VIRGULE
$v_2	identificateur	$v_2
=	symbole	EGAL
lire	mot_clef	lire
(	symbole	PARENTHESE_OUVRANTE
)	symbole	PARENTHESE_FERMANTE
;	symbole	POINT_VIRGULE
si	mot_clef	si
max	identificateur	max
(	symbole	PARENTHESE_OUVRANTE
$v_1	identificateur	$v_1
,	symbole	VIRGULE
$v_2	identificateur	$v_2
)	symbole	PARENTHESE_FERMANTE
=	symbole	EGAL
$v_1	identificateur	$v_1
alors	mot_clef	alors
{	symbole	ACCOLADE_OUVRANTE
ecrire	mot_clef	ecrire
(	symbole	PARENTHESE_OUVRANTE
$v_1	identificateur	$v_1
)	symbole	PARENTHESE_FERMANTE
;	symbole	POINT_VIRGULE
}	symbole	ACCOLADE_FERMANTE
sinon	mot_clef	sinon
{	symbole	ACCOLADE_OUVRANTE
ecrire	mot_clef	ecrire
(	symbole	PARENTHESE_OUVRANTE
$v_2	identificateur	$v_2
)	symbole	PARENTHESE_FERMANTE
;	symbole	POINT_VIRGULE
}	symbole	ACCOLADE_FERMANTE
}	symbole	ACCOLADE_FERMANTE

/**
 * This file is part of Patternshop Project.
 * 
 * Patternshop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Patternshop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Patternshop.  If not, see <http://www.gnu.org/licenses/>
*/
#ifdef _WINDOWS
# include "stdafx.h"
#else // _MACOSX
# include "PsMacTools.h"
#endif

#include <assert.h>
#include "PsMessage.h"

PsLanguage  currentLanguage = Default;

/**
 * Structure de message d'erreur
*/
typedef struct 
{
  ErrID errID;
  char *msg;
} ErrITEM;

/**
 * Structure de message d'avertissement
*/
typedef struct 
{
  QuestID questID;
  char *msg;
} QuestITEM;

/**
 * Structure pour un label
*/
typedef struct 
{
   LabelID labelID;
   char *msg;
} LabelITEM;

/**
 * Structure pour une action
*/
typedef struct 
{
   ActionID actionID;
   char *msg;
} ActionITEM;

/******************************************************************************
** Messages d'erreur en fran�ais
*/
static const ErrITEM frenchErrMsg[] =
{
	{ ERROR_FILE_ACCESS, "Impossible d'ouvrir le fichier choisi." },
	{ ERROR_FILE_READ, "Erreur de lecture dans le fichier (fichier corrompu)." },
	{ ERROR_FILE_VERSION, "Impossible d'ouvrir le fichier car le format n'est pas compatible avec cette version du logiciel."},
	{ ERROR_FILE_WRITE, "Erreur d'�criture dans le fichier (pas assez de m�moire)." },

	{ ERROR_IMAGE_LOAD, "Impossible de charger le motif (fichier invalide)." },
	{ ERROR_IMAGE_MATRIX, "Vous devez cr�er une matrice avant d'y ajouter des motis." },
	{ ERROR_IMAGE_SELECT, "Pour effectuer l'op�ration demand�e vous devez\nd'abord avoir s�lectionn� un motif." }, 

	{ ERROR_MATRIX_SELECT, "Pour effectuer l'op�ration demand�e vous devez\nd'abord avoir s�lectionn� une matrice,\nen ayant cliqu� dessus par exemple." },
	{ ERROR_MATRIX_NO_EXPORTABLE, "Pour effectuer la mise au raccord, aucune torsion ne doit �tre appliqu�e � la matrice." },

	{ ERROR_PATTERN_LOAD, "Impossible de charger le patron." },
	{ ERROR_PATTERN_SELECT, "Pour effectuer l'op�ration demand�e vous devez\nd'abord avoir s�lectionn� un patron." },
	{ ERROR_PATTERN_SELECT, "Pour effectuer l'op�ration demand�e vous devez\nd'abord avoir s�lectionn� un projet." },
		
	{ ERROR_VIDEOCARD, "Impossible de cr�er la m�moire de pixels.\nMerci de mettre � jour votre carte graphique." },

	{ ERROR_UNDEF, "Erreur inconnue" }

};

/*
** Messages d'avertissement en fran�ais
*/
static const QuestITEM frenchQuestMsg[] =
{
  { QUESTION_DELETE_IMAGE, "Vous allez supprimer un motif. �tes-vous s�r ?" },
  { QUESTION_DELETE_MATRIX, "Supprimer une matrice effacera �galement son contenu. �tes-vous s�r ?" },
  { QUESTION_EXPORT_ROTATION, "Afin de permettre l'export du racord,\nl'angle de celui-ci sera mis � plat.\nVoulez vous continuer ?" },
  { QUESTION_LOOSE_ALPHA, "Dans ce format vous ne conserverez pas\nla transparence de l'image.\nVoulez vous continuer ?" },
  { QUESTION_RESET_CONFIG_FILE, "Le fichier de configuration sera r�initialis� au prochain d�marrage du logiciel.\n�tes-vous s�r de vouloir continuer ?" },
  { QUESTION_SAVE_CLOSING, "Voulez-vous enregistrer les modifications avant de fermer le document ?" },
  { QUESTION_UNDEF, NULL }
};

/**
 * Label in french
*/
static const LabelITEM frenchLabelMsg[] =
{
   { LABEL_MATRIX, "Matrice" },
   { LABEL_IMAGE, "Motif" },
   { LABEL_PATTERN, "Patron" },
   { LABEL_BACKGROUND, "Arriere-plan" },
   { LABEL_UNDEF, NULL }
};

/**
 * Actions in french
*/
static const ActionITEM frenchActionMsg[] =
{
   { ACTION_DEL_IMAGE, "Suppression image" },
   { ACTION_DEL_MATRIX, "Suppression matrice" },
   { ACTION_MOVE, "D�placement" },
   { ACTION_NEW_IMAGE, "Nouvelle image" },
   { ACTION_NEW_MATRIX, "Nouvelle matrice" },
   { ACTION_REPLACE, "Remplacer image" },
   { ACTION_RESIZE, "Redimentionnement" },
   { ACTION_ROTATE, "Rotation" },
   { ACTION_SWAP_IMAGE, "Ordre des images" },
   { ACTION_SWAP_MATRIX, "Ordre des matrices" },
   { ACTION_TORSIO, "Torsion" },
   { ACTION_PATTERN_ROTATE, "Rotation du calque" },
   { ACTION_PATTERN_TRANSLATE, "Deplacement du calque" },
   { ACTION_PATTERN_SCALE, "Redimentionnement du calque" },
   { ACTION_UNDEF, NULL }
};

/**
 * Error messages in english
*/
static const ErrITEM englishErrMsg[] =
{
   { ERROR_FILE_ACCESS, "Unable to open selected file." },
   { ERROR_FILE_READ, "Error while reading the file (Corrupted file)." },
   { ERROR_FILE_VERSION, "Unable to open the file, unknown format version."},
   { ERROR_FILE_WRITE, "Unable to write the file." },

   { ERROR_IMAGE_LOAD, "Unable to load the image." },
   { ERROR_IMAGE_MATRIX, "You should create a pattern before adding an image." },
   { ERROR_IMAGE_SELECT, "To perform the requested action\nyou have first to select an image in the project." }, 

   { ERROR_MATRIX_SELECT, "To perform the requested action\nyou have first to select a pattern in the project." },
   { ERROR_MATRIX_NO_EXPORTABLE, "To export the pattern you have to remove its torsio." },

   { ERROR_PATTERN_LOAD, "Unable to load the template." },
   { ERROR_PATTERN_SELECT, "To perform the requested action\nyou have first to select a template in the project." },
   { ERROR_PATTERN_SELECT, "To perform the requested action\nyou have first to select a project." },

   { ERROR_VIDEOCARD, "Unable to create a pixel buffer.\nYou should probably update your video card driver." },
   
   { ERROR_UNDEF, "Unknown error" }

};

/**
 * Messages d'avertissement in english
*/
static const QuestITEM englishQuestMsg[] =
{
   { QUESTION_DELETE_IMAGE, "You are going to delete an image.\nDo you want to continue  ?" },
   { QUESTION_DELETE_MATRIX, "Delete a pattern from the project\nalso remove contained images.\nDo you want to continue  ?" },
   { QUESTION_EXPORT_ROTATION, "To export the pattern, a rotation have to be done.\nDo you want to continue ?" },
   { QUESTION_LOOSE_ALPHA, "This file format doesn't manage\nthe transparency.\nDo you want to continue ?" },
   { QUESTION_RESET_CONFIG_FILE, "The configuration file will be\nrestored to default values after restart.\nDo you want to continue ?" },
   { QUESTION_SAVE_CLOSING, "Do you want to save the modification before closing the document ?" },
   { QUESTION_UNDEF, NULL }
};


/**
 * Label in english
*/
static const LabelITEM englishLabelMsg[] =
{
   { LABEL_MATRIX, "Pattern" },
   { LABEL_IMAGE, "Image" },
   { LABEL_PATTERN, "Template" },
   { LABEL_BACKGROUND, "Background" },
   { LABEL_UNDEF, NULL }
};

/**
 * Actions in english
*/
static const ActionITEM englishActionMsg[] =
{
   { ACTION_DEL_IMAGE, "Delete image" },
   { ACTION_DEL_MATRIX, "Delete matrice" },
   { ACTION_MOVE, "Translation" },
   { ACTION_NEW_IMAGE, "New image" },
   { ACTION_NEW_MATRIX, "New matrice" },
   { ACTION_REPLACE, "Replace image" },
   { ACTION_RESIZE, "Resize" },
   { ACTION_ROTATE, "Rotation" },
   { ACTION_SWAP_IMAGE, "Swap images order" },
   { ACTION_SWAP_MATRIX, "Swap matrix order" },
   { ACTION_TORSIO, "Torsio" },
   { ACTION_PATTERN_ROTATE, "Template rotation" },
   { ACTION_PATTERN_TRANSLATE, "Template translation" },
   { ACTION_PATTERN_SCALE, "Template scaling" },
   { ACTION_UNDEF, NULL }
};

ErrITEM*    ErrMsg = ( ErrITEM* )englishErrMsg;
QuestITEM*  QuestMsg = ( QuestITEM* )englishQuestMsg;
LabelITEM*  LabelMsg = ( LabelITEM* )englishLabelMsg;
ActionITEM* ActionMsg = ( ActionITEM* )englishActionMsg;

void setLanguage( PsLanguage language )
{
   switch ( language )
   {
      case French:
         currentLanguage = French;
         ErrMsg = ( ErrITEM* )frenchErrMsg;
         QuestMsg = ( QuestITEM* )frenchQuestMsg;
         LabelMsg = ( LabelITEM* )frenchLabelMsg;
         ActionMsg = ( ActionITEM* )frenchActionMsg;
         break;

      default:
         currentLanguage = English;
         ErrMsg = ( ErrITEM* )englishErrMsg;
         QuestMsg = ( QuestITEM* )englishQuestMsg;
         LabelMsg = ( LabelITEM* )englishLabelMsg;
         ActionMsg = ( ActionITEM* )englishActionMsg;
         break;
   }
}

PsLanguage getLanguage()
{
   return currentLanguage;
}

/****************************************************************************
** M�thodes d'acc�s aux messages d'erreur
*/
void GetError(ErrID id)
{
	int i = 0;
	char* msg;
	
	//-- tout est OK
	if (id == ERROR_NONE)
		return;
	//--
	
	//-- selection du message
	for (; ErrMsg[i].errID != ERROR_UNDEF; ++i)
		if (ErrMsg[i].errID == id)
			break;
	msg = ErrMsg[i].msg;
	//--
	
	#ifdef _WINDOWS
	MessageBox(0, msg, "", MB_ICONINFORMATION);
	#else  /* _MACOSX */
	PsMessageBox(msg);
	#endif
}

/*
** M�thodes d'acc�s aux messages d'avertissement
*/
bool GetQuestion(QuestID id)
{
	int i = 0;
	char*	msg;
	
	//-- selection du message
	for (; QuestMsg[i].questID != QUESTION_UNDEF; ++i)
		if (QuestMsg[i].questID == id)
			break;
	assert(QuestMsg[i].questID != QUESTION_UNDEF);
	msg = QuestMsg[i].msg;
	//--
	
	#ifdef _WINDOWS
	return MessageBox (0, msg, "", MB_ICONQUESTION | MB_YESNO) == IDYES;
	#else  /* _MACOSX */
	return PsQuestionBox(msg);
	#endif
	
	//-- �a devrait pas arriver
	assert(false);
	return true;
	//--
}

/*
** M�thodes d'acc�s aux labels
*/
char* GetLabel(LabelID id)
{
   int i = 0;
   char*	msg;

   //-- selection du message
   for (; LabelMsg[i].labelID != LABEL_UNDEF; ++i)
      if (LabelMsg[i].labelID == id)
         break;
   assert(LabelMsg[i].labelID != LABEL_UNDEF);
   msg = LabelMsg[i].msg;
   //--

   return msg;
}

/*
** M�thodes d'acc�s aux actions
*/
char* GetAction( ActionID id )
{
   int i = 0;
   char*	msg;

   //-- selection du message
   for (; ActionMsg[i].actionID != ACTION_UNDEF; ++i)
      if (ActionMsg[i].actionID == id)
         break;
   assert(ActionMsg[i].actionID != ACTION_UNDEF);
   msg = ActionMsg[i].msg;
   //--

   return msg;
}

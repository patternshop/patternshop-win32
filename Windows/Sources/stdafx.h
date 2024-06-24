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
#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#ifndef MACOSX
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclure les en-têtes Windows rarement utilisés
#endif

// Modifiez les définitions suivantes si vous devez cibler une plate-forme avant celles spécifiées ci-dessous.
// Reportez-vous à MSDN pour obtenir les dernières informations sur les valeurs correspondantes pour les différentes plates-formes.
#ifndef WINVER				// Autorise l'utilisation des fonctionnalités spécifiques à Windows 95 et Windows NT 4 ou version ultérieure.
#define WINVER 0x0400		// Attribuez la valeur appropriée à cet élément pour cibler Windows 98 et Windows 2000 ou version ultérieure.
#endif

#ifndef _WIN32_WINNT		// Autorise l'utilisation des fonctionnalités spécifiques à Windows NT 4 ou version ultérieure.
#define _WIN32_WINNT 0x0400		// Attribuez la valeur appropriée à cet élément pour cibler Windows 98 et Windows 2000 ou version ultérieure.
#endif						

#ifndef _WIN32_WINDOWS		// Autorise l'utilisation des fonctionnalités spécifiques à Windows 98 ou version ultérieure.
#define _WIN32_WINDOWS 0x0410 // Attribuez la valeur appropriée à cet élément pour cibler Windows Me ou version ultérieure.
#endif

#ifndef _WIN32_IE			// Autorise l'utilisation des fonctionnalités spécifiques à IE 4.0 ou version ultérieure.
#define _WIN32_IE 0x0400	// Attribuez la valeur appropriée à cet élément pour cibler IE 5.0 ou version ultérieure.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// certains constructeurs CString seront explicites

// désactive le masquage MFC de certains messages d'avertissement courants et par ailleurs souvent ignorés
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // composants MFC principaux et standard
#include <afxext.h>         // extensions MFC
#include <afxdisp.h>        // Classes MFC Automation

#include <afxdtctl.h>		// Prise en charge des MFC pour les contrôles communs Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Prise en charge des MFC pour les contrôles communs Windows
#endif // _AFX_NO_AFXCMN_SUPPORT
#endif
#include <afxdlgs.h>

/*
 * Copyright 2023 NXP.
 * NXP Confidential. This software is owned or controlled by NXP and may only be used strictly in accordance
 * with the license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef DSMT_FR_FR_STRINGS_DSMT_H_
#define DSMT_FR_FR_STRINGS_DSMT_H_

#if ENABLE_DSMT_ASR

/* French demos strings below.
 * These must match the DSMTv2 groups */

const char * const ww_fr[] = {
        "Salut NXP",
        "Salut NXP ^1",
};

const char * const cmd_change_demo_fr[] = {
        "Ascenseur",
        "Lave-Linge",
        "Maison Connectée",
};

const char * const cmd_elevator_fr[] = {
        "Premier étage",
        "Deuxième étage",
        "Troisième étage",
        "Quatrième étage",
        "Cinquième étage",
        "Entrée principale",
        "Rez-de-chaussée",
        "Sous-sol",
        "Ouvrir porte",
        "Ouvrir porte ^1",
        "Fermer porte",
        "Fermer porte ^1",
        "Changer de démo",
};

const char * const cmd_washing_machine_fr[] = {
        "Lavage délicat",
        "Lavage normal",
        "Lavage en profondeur",
        "Lavage blanc",
        "Commencer",
        "Annuler",
        "Changer de démo",
};

const char * const cmd_smart_home_fr[] = {
        "Allumer lumière",
        "Éteindre lumière",
        "Augmenter température",
        "Diminuer température",
        "Ouvrir fenêtre",
        "Fermer fenêtre",
        "Augmenter luminosité",
        "Diminuer luminosité",
        "Changer de démo",
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_FR_FR_STRINGS_DSMT_H_ */

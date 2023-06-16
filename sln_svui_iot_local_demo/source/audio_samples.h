/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2021 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AUDIO_SAMPLES_H_
#define AUDIO_SAMPLES_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TONE_BOOT_SIZE    21257
#define TONE_TIMEOUT_SIZE 21319

#define AUDIO_DEMO_CLIP_SIZE      117648
#define DIALOG_DEMO_CLIP_SIZE     46080
#define ELEVATOR_DEMO_CLIP_SIZE   87120
#define LED_DEMO_CLIP_SIZE        49536
#define SMART_HOME_DEMO_CLIP_SIZE 94080
#define WASH_DEMO_CLIP_SIZE       103584

/*******************************************************************************
 * Global Vars
 ******************************************************************************/
extern const signed char tone_boot[TONE_BOOT_SIZE];
extern const signed char tone_timeout[TONE_TIMEOUT_SIZE];

extern const short audio_demo_clip[AUDIO_DEMO_CLIP_SIZE];
extern const short dialog_demo_clip[DIALOG_DEMO_CLIP_SIZE];
extern const short elevator_demo_clip[ELEVATOR_DEMO_CLIP_SIZE];
extern const short led_demo_clip[LED_DEMO_CLIP_SIZE];
extern const short smart_home_demo_clip[SMART_HOME_DEMO_CLIP_SIZE];
extern const short wash_demo_clip[WASH_DEMO_CLIP_SIZE];

/*******************************************************************************
 * End
 ******************************************************************************/

#endif /* AUDIO_SAMPLES_H_ */

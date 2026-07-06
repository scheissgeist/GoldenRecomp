#include "patches.h"

RECOMP_PATCH f32 alCents2Ratio(s32 cents) {
    f32 x;
    f32 ratio = 1.0f;

    if (cents >= 0) {
        x = 1.00057779f;
    } else {
        x = 0.9994225441f;
        cents = -cents;
    }

    while (cents) {
        if (cents & 1) {
            ratio *= x;
        }
        x *= x;
        cents >>= 1;
    }

    return ratio;
}

#if 1
RECOMP_PATCH void musicTrack1Play(s32 track) __attribute__((optnone)) {
    u32 trackSizeBytes;
    struct music_struct_b thing;
    u8* temp_a0;
    void* romAddress;
    u32 t3;
    struct huft hlist;

    if (g_sndBootswitchSound) {
        return;
    }

    if (g_musicXTrack1CurrentTrackNum) {
        musicTrack1Stop();
    }

    g_musicXTrack1CurrentTrackNum = track;

    while (alCSPGetState(g_musicXTrack1SeqPlayer)) {
        // @recomp: Yield the thread so it doesn't stall
        yield_self_1ms();
    }

    romAddress = g_musicDataTable->seqArray[g_musicXTrack1CurrentTrackNum].address;

    if (romAddress < (void*) ROM_MUSIC_START_OFFSET) {
        // Note: recursive call
        musicTrack1Play(M_SHORT_SOLO_DEATH);

        return;
    }

    t3 = ALIGN16_a(g_musicTrackLength[g_musicXTrack1CurrentTrackNum]) + ALIGN16_a(NUM_MUSIC_TRACKS);
    trackSizeBytes = ALIGN16_a(g_musicTrackCompressedLength[g_musicXTrack1CurrentTrackNum]);
    thing.seqData = ge_audio_dump_enabled_native() ? (u8*)0x80300000 : g_musicXTrack1SeqData;
    temp_a0 = (u8*) ((t3 + (s32) thing.seqData) - trackSizeBytes);

    recomp_printf("GE_AUDIO_DUMP_PATCH: track1 track=%d rom=%08X len=%08X clen=%08X temp=%08X seq=%08X\n",
        track,
        (u32) romAddress,
        g_musicTrackLength[g_musicXTrack1CurrentTrackNum],
        g_musicTrackCompressedLength[g_musicXTrack1CurrentTrackNum],
        (u32) temp_a0,
        (u32) thing.seqData);
    romCopy(temp_a0, romAddress, trackSizeBytes);
    recomp_printf("GE_AUDIO_DUMP_PATCH: track1 after romCopy\n");
    decompressdata(temp_a0, thing.seqData, &hlist);
    recomp_printf("GE_AUDIO_DUMP_PATCH: track1 after decompress\n");

    alCSeqNew(&g_musicXTrack1Seq, thing.seqData);
    recomp_printf("GE_AUDIO_DUMP_PATCH: track1 after alCSeqNew\n");
    alCSPSetSeq(g_musicXTrack1SeqPlayer, &g_musicXTrack1Seq);
    recomp_printf("GE_AUDIO_DUMP_PATCH: track1 after alCSPSetSeq\n");
    if (ge_audio_dump_enabled_native()) {
        u32 volume = 0x7FFF;
        int seqp_shift = ge_audio_dump_seqp_shift_native();
        if (seqp_shift > 0) {
            volume >>= seqp_shift;
        }
        alCSPSetVol(
            g_musicXTrack1SeqPlayer,
            (s16)((volume * (u32)ge_audio_dump_music_default_volume_native(track)) >> 15));
    } else {
        musicTrack1ApplySeqpVol(musicTrack1GetVolume());
    }
    recomp_printf("GE_AUDIO_DUMP_PATCH: track1 after volume\n");
    alCSPPlay(g_musicXTrack1SeqPlayer);
    recomp_printf("GE_AUDIO_DUMP_PATCH: track1 after play\n");
}
#endif

#if 1
RECOMP_PATCH void musicTrack2Play(s32 track) __attribute__((optnone)) {
    u32 trackSizeBytes;
    struct music_struct_b thing;
    u8* temp_a0;
    void* romAddress;
    u32 t3;
    struct huft hlist;

    if (g_sndBootswitchSound) {
        return;
    }

    if (g_musicXTrack2CurrentTrackNum) {
        musicTrack2Stop();
    }

    g_musicXTrack2CurrentTrackNum = track;

    while (alCSPGetState(g_musicXTrack2SeqPlayer)) {
        // @recomp: Yield the thread so it doesn't stall
        yield_self_1ms();
    }

    romAddress = g_musicDataTable->seqArray[g_musicXTrack2CurrentTrackNum].address;

    if (romAddress < (void*) ROM_MUSIC_START_OFFSET) {
        // Note: recursive call
        musicTrack2Play(M_SHORT_SOLO_DEATH);

        return;
    }

    t3 = ALIGN16_a(g_musicTrackLength[g_musicXTrack2CurrentTrackNum]) + ALIGN16_a(NUM_MUSIC_TRACKS);
    trackSizeBytes = ALIGN16_a(g_musicTrackCompressedLength[g_musicXTrack2CurrentTrackNum]);
    thing.seqData = g_musicXTrack2SeqData;
    temp_a0 = (u8*) ((t3 + (s32) thing.seqData) - trackSizeBytes);

    romCopy(temp_a0, romAddress, trackSizeBytes);
    decompressdata(temp_a0, thing.seqData, &hlist);

    alCSeqNew(&g_musicXTrack2Seq, g_musicXTrack2SeqData);
    alCSPSetSeq(g_musicXTrack2SeqPlayer, &g_musicXTrack2Seq);
    musicTrack2ApplySeqpVol(musicTrack2GetVolume());
    alCSPPlay(g_musicXTrack2SeqPlayer);
}
#endif

#if 1
RECOMP_PATCH void musicTrack3Play(s32 track) __attribute__((optnone)) {
    u32 trackSizeBytes;
    struct music_struct_b thing;
    u8* temp_a0;
    void* romAddress;
    u32 t3;
    struct huft hlist;

    if (g_sndBootswitchSound) {
        return;
    }

    if (g_musicXTrack3CurrentTrackNum) {
        musicTrack3Stop();
    }

    g_musicXTrack3CurrentTrackNum = track;

    while (alCSPGetState(g_musicXTrack3SeqPlayer)) {
        // @recomp: Yield the thread so it doesn't stall
        yield_self_1ms();
    }

    romAddress = g_musicDataTable->seqArray[g_musicXTrack3CurrentTrackNum].address;

    if (romAddress < (void*) ROM_MUSIC_START_OFFSET) {
        // Note: recursive call
        musicTrack3Play(M_SHORT_SOLO_DEATH);

        return;
    }

    t3 = ALIGN16_a(g_musicTrackLength[g_musicXTrack3CurrentTrackNum]) + ALIGN16_a(NUM_MUSIC_TRACKS);
    trackSizeBytes = ALIGN16_a(g_musicTrackCompressedLength[g_musicXTrack3CurrentTrackNum]);
    thing.seqData = g_musicXTrack3SeqData;
    temp_a0 = (u8*) ((t3 + (s32) thing.seqData) - trackSizeBytes);

    romCopy(temp_a0, romAddress, trackSizeBytes);
    decompressdata(temp_a0, thing.seqData, &hlist);

    alCSeqNew(&g_musicXTrack3Seq, g_musicXTrack3SeqData);
    alCSPSetSeq(g_musicXTrack3SeqPlayer, &g_musicXTrack3Seq);
    musicTrack3ApplySeqpVol(musicTrack3GetVolume());
    alCSPPlay(g_musicXTrack3SeqPlayer);
}
#endif

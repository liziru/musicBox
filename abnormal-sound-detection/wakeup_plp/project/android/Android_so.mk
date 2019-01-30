LOCAL_PATH := $(call my-dir)  
BASE_PATH := $(LOCAL_PATH)../src/sphinxbase
POCKET_PATH := $(LOCAL_PATH)../src/pocketsphinx
include $(CLEAR_VARS)  

LOCAL_ARM_MODE  := arm
PRE_DEF := -DHAVE_CONFIG_H
OPT_CFLAGS := -O3 -ffunction-sections -fdata-sections
OPT_LDFLAGS := -O3 -Wl,--gc-sections 

LOCAL_CFLAGS := $(LOCAL_CFLAGS) $(OPT_CFLAGS) $(PRE_DEF)
LOCAL_LDFLAGS := $(LOCAL_LDFLAGS) $(OPT_LDFLAGS)
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -std=c++11
LOCAL_MODULE    := TwirlingWakeUp
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += -Wno-format-security
LOCAL_SRC_FILES :=  \
$(BASE_PATH)/src/libsphinxbase/fe/fe_interface.c                       \
$(BASE_PATH)/src/libsphinxbase/fe/fe_noise.c                           \
$(BASE_PATH)/src/libsphinxbase/fe/fe_prespch_buf.c                     \
$(BASE_PATH)/src/libsphinxbase/fe/fe_sigproc.c                         \
$(BASE_PATH)/src/libsphinxbase/fe/fe_warp.c                            \
$(BASE_PATH)/src/libsphinxbase/fe/fe_warp_affine.c                     \
$(BASE_PATH)/src/libsphinxbase/fe/fe_warp_inverse_linear.c             \
$(BASE_PATH)/src/libsphinxbase/fe/fe_warp_piecewise_linear.c           \
$(BASE_PATH)/src/libsphinxbase/fe/fixlog.c                             \
$(BASE_PATH)/src/libsphinxbase/fe/yin.c                                \
$(BASE_PATH)/src/libsphinxbase/feat/agc.c                              \
$(BASE_PATH)/src/libsphinxbase/feat/cmn.c                              \
$(BASE_PATH)/src/libsphinxbase/feat/cmn_live.c                         \
$(BASE_PATH)/src/libsphinxbase/feat/feat.c                             \
$(BASE_PATH)/src/libsphinxbase/feat/lda.c                              \
$(BASE_PATH)/src/libsphinxbase/lm/fsg_model.c                          \
$(BASE_PATH)/src/libsphinxbase/lm/jsgf.c                               \
$(BASE_PATH)/src/libsphinxbase/lm/jsgf_parser.c                        \
$(BASE_PATH)/src/libsphinxbase/lm/jsgf_scanner.c                       \
$(BASE_PATH)/src/libsphinxbase/lm/lm_trie.c                            \
$(BASE_PATH)/src/libsphinxbase/lm/lm_trie_quant.c                      \
$(BASE_PATH)/src/libsphinxbase/lm/ngram_model.c                        \
$(BASE_PATH)/src/libsphinxbase/lm/ngram_model_set.c                    \
$(BASE_PATH)/src/libsphinxbase/lm/ngram_model_trie.c                   \
$(BASE_PATH)/src/libsphinxbase/lm/ngrams_raw.c                         \
$(BASE_PATH)/src/libsphinxbase/util/bio.c                              \
$(BASE_PATH)/src/libsphinxbase/util/bitarr.c                           \
$(BASE_PATH)/src/libsphinxbase/util/bitvec.c                           \
$(BASE_PATH)/src/libsphinxbase/util/blas_lite.c                        \
$(BASE_PATH)/src/libsphinxbase/util/case.c                             \
$(BASE_PATH)/src/libsphinxbase/util/ckd_alloc.c                        \
$(BASE_PATH)/src/libsphinxbase/util/cmd_ln.c                           \
$(BASE_PATH)/src/libsphinxbase/util/dtoa.c                             \
$(BASE_PATH)/src/libsphinxbase/util/err.c                              \
$(BASE_PATH)/src/libsphinxbase/util/errno.c                            \
$(BASE_PATH)/src/libsphinxbase/util/f2c_lite.c                         \
$(BASE_PATH)/src/libsphinxbase/util/filename.c                         \
$(BASE_PATH)/src/libsphinxbase/util/genrand.c                          \
$(BASE_PATH)/src/libsphinxbase/util/glist.c                            \
$(BASE_PATH)/src/libsphinxbase/util/hash_table.c                       \
$(BASE_PATH)/src/libsphinxbase/util/heap.c                             \
$(BASE_PATH)/src/libsphinxbase/util/listelem_alloc.c                   \
$(BASE_PATH)/src/libsphinxbase/util/logmath.c                          \
$(BASE_PATH)/src/libsphinxbase/util/matrix.c                           \
$(BASE_PATH)/src/libsphinxbase/util/mmio.c                             \
$(BASE_PATH)/src/libsphinxbase/util/pio.c                              \
$(BASE_PATH)/src/libsphinxbase/util/priority_queue.c                   \
$(BASE_PATH)/src/libsphinxbase/util/profile.c                          \
$(BASE_PATH)/src/libsphinxbase/util/sbthread.c                         \
$(BASE_PATH)/src/libsphinxbase/util/slamch.c                           \
$(BASE_PATH)/src/libsphinxbase/util/slapack_lite.c                     \
$(BASE_PATH)/src/libsphinxbase/util/strfuncs.c                         \
$(POCKET_PATH)/src/libpocketsphinx/acmod.c                             \
$(POCKET_PATH)/src/libpocketsphinx/allphone_search.c                   \
$(POCKET_PATH)/src/libpocketsphinx/bin_mdef.c                          \
$(POCKET_PATH)/src/libpocketsphinx/blkarray_list.c                     \
$(POCKET_PATH)/src/libpocketsphinx/dict.c                              \
$(POCKET_PATH)/src/libpocketsphinx/dict2pid.c                          \
$(POCKET_PATH)/src/libpocketsphinx/fsg_history.c                       \
$(POCKET_PATH)/src/libpocketsphinx/fsg_lextree.c                       \
$(POCKET_PATH)/src/libpocketsphinx/fsg_search.c                        \
$(POCKET_PATH)/src/libpocketsphinx/hmm.c                               \
$(POCKET_PATH)/src/libpocketsphinx/kws_detections.c                    \
$(POCKET_PATH)/src/libpocketsphinx/kws_search.c                        \
$(POCKET_PATH)/src/libpocketsphinx/mdef.c                              \
$(POCKET_PATH)/src/libpocketsphinx/ms_gauden.c                         \
$(POCKET_PATH)/src/libpocketsphinx/ms_mgau.c                           \
$(POCKET_PATH)/src/libpocketsphinx/ms_senone.c                         \
$(POCKET_PATH)/src/libpocketsphinx/ngram_search.c                      \
$(POCKET_PATH)/src/libpocketsphinx/ngram_search_fwdflat.c              \
$(POCKET_PATH)/src/libpocketsphinx/ngram_search_fwdtree.c              \
$(POCKET_PATH)/src/libpocketsphinx/phone_loop_search.c                 \
$(POCKET_PATH)/src/libpocketsphinx/pocketsphinx.c                      \
$(POCKET_PATH)/src/libpocketsphinx/ps_alignment.c                      \
$(POCKET_PATH)/src/libpocketsphinx/ps_lattice.c                        \
$(POCKET_PATH)/src/libpocketsphinx/ps_mllr.c                           \
$(POCKET_PATH)/src/libpocketsphinx/ptm_mgau.c                          \
$(POCKET_PATH)/src/libpocketsphinx/s2_semi_mgau.c                      \
$(POCKET_PATH)/src/libpocketsphinx/state_align_search.c                \
$(POCKET_PATH)/src/libpocketsphinx/tmat.c                              \
$(POCKET_PATH)/src/libpocketsphinx/vector.c                            \
$(POCKET_PATH)/src/libpocketsphinx/wakeupCore.cpp                      \
$(POCKET_PATH)/src/libpocketsphinx/TwirlingWakeupApi.cpp               \
./com_twirling_audio_Wakeup.cpp 

LOCAL_C_INCLUDES := \
$(BASE_PATH)/include  \
$(BASE_PATH)/include/android \
$(BASE_PATH)/include/sphinxbase \
$(POCKET_PATH)/include

include $(BUILD_SHARED_LIBRARY)

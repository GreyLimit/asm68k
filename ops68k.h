static opcode opcode_104 = { SIZE_B, NUM_AREG_DEC_IND, NUM_AREG_DEC_IND, 0xC108, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_103 = { SIZE_B, NUM_DREG, NUM_DREG, 0xC100, 0x0000, 0, 0, 9, FLAG_68000, &opcode_104 };
static opcode opcode_109 = { SIZE_BWL, EA_SOURCE_ARG, NUM_DREG, 0xD000, 0x0000, 6, 0, 9, FLAG_68000, NULL };
static opcode opcode_108 = { SIZE_BWL, NUM_DREG, EA_MEMORY_ARG, 0xD100, 0x0000, 6, 9, 0, FLAG_68000, &opcode_109 };
static opcode opcode_111 = { SIZE_BWL, EA_IMMEDIATE, EA_DEST_ARG, 0x0600, 0x0000, 6, APPEND, 0, FLAG_68000, NULL };
static opcode opcode_110 = { SIZE_WL, EA_SOURCE_ARG, NUM_AREG, 0xD0C0, 0x0000, 8, 0, 9, FLAG_68000, NULL };
static opcode opcode_114 = { SIZE_BWL, NUM_AREG_DEC_IND, NUM_AREG_DEC_IND, 0xD108, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_113 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xD100, 0x0000, 6, 9, 0, FLAG_68000, &opcode_114 };
static opcode opcode_120 = { SIZE_BWL, EA_IMMEDIATE, EA_DEST_ARG, 0x0200, 0x0000, 6, APPEND, 0, FLAG_68000, NULL };
static opcode opcode_119 = { SIZE_BWL, EA_SOURCE_ARG, NUM_DREG, 0xC000, 0x0000, 6, 0, 9, FLAG_68000, NULL };
static opcode opcode_118 = { SIZE_BWL, NUM_DREG, EA_MEMORY_ARG, 0xC100, 0x0000, 6, 9, 0, FLAG_68000, &opcode_119 };
static opcode opcode_112 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, EA_DEST_ARG|EA_AREG, 0x5000, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_128 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xE020, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_129 = { SIZE_W, EA_MEMORY_ARG, NO_ARGUMENT, 0xE0C0, 0x0000, 0, 0, 0, FLAG_68000, &opcode_128 };
static opcode opcode_127 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, NUM_DREG, 0xE000, 0x0000, 6, 9, 0, FLAG_68000, &opcode_129 };
static opcode opcode_160 = { SIZE_L, NUM_DREG, EA_DREG, 0x0140, 0x0000, 0, 9, 0, FLAG_68000, NULL };
static opcode opcode_161 = { SIZE_B, NUM_IMMEDIATE_3BIT, EA_MEMORY_ARG, 0x0840, 0x0000, 0, APPEND, 0, FLAG_68000, &opcode_160 };
static opcode opcode_162 = { SIZE_L, NUM_IMMEDIATE_5BIT, EA_DREG, 0x0840, 0x0000, 0, APPEND, 0, FLAG_68000, &opcode_161 };
static opcode opcode_159 = { SIZE_B, NUM_DREG, EA_MEMORY_ARG, 0x0140, 0x0000, 0, 9, 0, FLAG_68000, &opcode_162 };
static opcode opcode_142 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6400, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_144 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6500, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_152 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6C00, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_147 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6700, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_165 = { SIZE_L, NUM_DREG, EA_DREG, 0x0180, 0x0000, 0, 9, 0, FLAG_68000, NULL };
static opcode opcode_166 = { SIZE_B, NUM_IMMEDIATE_3BIT, EA_MEMORY_ARG, 0x0880, 0x0000, 0, APPEND, 0, FLAG_68000, &opcode_165 };
static opcode opcode_167 = { SIZE_L, NUM_IMMEDIATE_5BIT, EA_DREG, 0x0880, 0x0000, 0, APPEND, 0, FLAG_68000, &opcode_166 };
static opcode opcode_164 = { SIZE_B, NUM_DREG, EA_MEMORY_ARG, 0x0180, 0x0000, 0, 9, 0, FLAG_68000, &opcode_167 };
static opcode opcode_125 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xE120, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_126 = { SIZE_W, EA_MEMORY_ARG, NO_ARGUMENT, 0xE1C0, 0x0000, 0, 0, 0, FLAG_68000, &opcode_125 };
static opcode opcode_124 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, NUM_DREG, 0xE100, 0x0000, 6, 9, 0, FLAG_68000, &opcode_126 };
static opcode opcode_140 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6200, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_155 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6F00, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_143 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6400, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_141 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6300, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_151 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6B00, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_153 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6D00, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_145 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6500, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_150 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6A00, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_170 = { SIZE_L, NUM_DREG, EA_DREG, 0x01C0, 0x0000, 0, 9, 0, FLAG_68000, NULL };
static opcode opcode_171 = { SIZE_B, NUM_IMMEDIATE_3BIT, EA_MEMORY_ARG, 0x08C0, 0x0000, 0, APPEND, 0, FLAG_68000, &opcode_170 };
static opcode opcode_172 = { SIZE_L, NUM_IMMEDIATE_5BIT, EA_DREG, 0x08C0, 0x0000, 0, APPEND, 0, FLAG_68000, &opcode_171 };
static opcode opcode_169 = { SIZE_B, NUM_DREG, EA_MEMORY_ARG, 0x01C0, 0x0000, 0, 9, 0, FLAG_68000, &opcode_172 };
static opcode opcode_138 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6000, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_175 = { SIZE_L, NUM_DREG, EA_DREG, 0x0100, 0x0000, 0, 9, 0, FLAG_68000, NULL };
static opcode opcode_176 = { SIZE_B, NUM_IMMEDIATE_3BIT, EA_MEMORY_ARG, 0x0800, 0x0000, 0, APPEND, 0, FLAG_68000, &opcode_175 };
static opcode opcode_177 = { SIZE_L, NUM_IMMEDIATE_5BIT, EA_DREG, 0x0800, 0x0000, 0, APPEND, 0, FLAG_68000, &opcode_176 };
static opcode opcode_174 = { SIZE_B, NUM_DREG, EA_MEMORY_ARG, 0x0100, 0x0000, 0, 9, 0, FLAG_68000, &opcode_177 };
static opcode opcode_149 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6900, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_148 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6800, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_139 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6100, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_146 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6600, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_154 = { NO_SIZE, ABS_ADDRESS, NO_ARGUMENT, 0x6E00, 0x0000, 0, 0, 0, FLAG_68000|FLAG_8_16_REL, NULL };
static opcode opcode_181 = { SIZE_BWL, EA_DEST_ARG, NO_ARGUMENT, 0x4200, 0x0000, 6, 0, 0, FLAG_68000, NULL };
static opcode opcode_184 = { SIZE_WL, EA_SOURCE_ARG, NUM_AREG, 0xB0C0, 0x0000, 8, 0, 9, FLAG_68000, NULL };
static opcode opcode_183 = { SIZE_BWL, EA_SOURCE_ARG, NUM_DREG, 0xB000, 0x0000, 6, 0, 9, FLAG_68000, NULL };
static opcode opcode_186 = { SIZE_BWL, NUM_AREG_IND_INC, NUM_AREG_IND_INC, 0xB108, 0x0000, 6, 0, 9, FLAG_68000, NULL };
static opcode opcode_202 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x55C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_200 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x54C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_185 = { SIZE_BWL, EA_IMMEDIATE, EA_DEST_ARG, 0x0C00, 0x0000, 6, APPEND, 0, FLAG_68000, NULL };
static opcode opcode_196 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x51C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_212 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x5EC8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_210 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x5CC8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_201 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x54C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_203 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x55C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_213 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x5FC8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_198 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x52C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_205 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x57C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_211 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x5DC8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_204 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x56C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_209 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x5BC8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_197 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x51C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_206 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x58C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_195 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x50C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_208 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x5AC8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_217 = { SIZE_W, EA_SOURCE_ARG, NUM_DREG, 0x81C0, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_222 = { SIZE_BWL, NUM_DREG, EA_DEST_ARG, 0xB100, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_218 = { SIZE_W, EA_SOURCE_ARG, NUM_DREG, 0x80C0, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_230 = { SIZE_L, NUM_AREG, NUM_AREG, 0xC148, 0x0000, 0, 9, 0, FLAG_68000, NULL };
static opcode opcode_231 = { SIZE_L, NUM_DREG, NUM_AREG, 0xC188, 0x0000, 0, 9, 0, FLAG_68000, &opcode_230 };
static opcode opcode_229 = { SIZE_L, NUM_DREG, NUM_DREG, 0xC140, 0x0000, 0, 9, 0, FLAG_68000, &opcode_231 };
static opcode opcode_239 = { NO_SIZE, NO_ARGUMENT, NO_ARGUMENT, 0x4AFC, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_235 = { SIZE_WL, NUM_DREG, NO_ARGUMENT, 0x4800, 0x0000, 6, 0, 0, FLAG_68000, NULL };
static opcode opcode_224 = { SIZE_BWL, EA_IMMEDIATE, EA_DEST_ARG, 0x0A00, 0x0000, 6, APPEND, 0, FLAG_68000, NULL };
static opcode opcode_207 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x59C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_199 = { SIZE_W, NUM_DREG, ABS_ADDRESS, 0x53C8, 0x0000, 0, 0, APPEND, FLAG_68000|FLAG_16_REL, NULL };
static opcode opcode_179 = { SIZE_W, EA_SOURCE_ARG, NUM_DREG, 0x4180, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_244 = { NO_SIZE, EA_ADDRESS_ARG, NO_ARGUMENT, 0x4E80, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_248 = { NO_SIZE, EA_ADDRESS_ARG, NUM_AREG, 0x41C0, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_257 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, NUM_DREG, 0xE108, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_258 = { SIZE_W, EA_MEMORY_ARG, NO_ARGUMENT, 0xE3C0, 0x0000, 0, 0, 0, FLAG_68000, &opcode_257 };
static opcode opcode_256 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xE128, 0x0000, 6, 9, 0, FLAG_68000, &opcode_258 };
static opcode opcode_252 = { NO_SIZE, NUM_AREG, NUM_IMMEDIATE_16, 0x4E50, 0x0000, 0, 0, APPEND, FLAG_68000, NULL };
static opcode opcode_266 = { SIZE_W, EA_SOURCE_ARG, EA_FLAGS_REG, 0x44C0, 0x0000, 0, 0, IGNORE, FLAG_68000, NULL };
static opcode opcode_267 = { SIZE_W, EA_SOURCE_ARG, EA_STATUS_REG, 0x46C0, 0x0000, 0, 0, IGNORE, FLAG_68000, &opcode_266 };
static opcode opcode_268 = { SIZE_W, EA_STATUS_REG, EA_DEST_ARG, 0x40C0, 0x0000, 0, IGNORE, 0, FLAG_68000, &opcode_267 };
static opcode opcode_269 = { SIZE_L, CONTROL_REG, NUM_AREG, 0x4E64, 0x0000, 0, IGNORE, 0, FLAG_68000|FLAG_USP_ONLY, &opcode_268 };
static opcode opcode_270 = { SIZE_L, NUM_AREG, CONTROL_REG, 0x4E60, 0x0000, 0, 0, IGNORE, FLAG_68000|FLAG_USP_ONLY, &opcode_269 };
static opcode opcode_423 = { SIZE_W, EA_FLAGS_REG, EA_DEST_ARG, 0x42C0, 0x0000, 0, IGNORE, 0, FLAG_68010, &opcode_270 };
static opcode opcode_265 = { SIZE_BWL, EA_SOURCE_ARG, EA_DEST_ARG, 0x0000, 0x0000, 12, 0, 9, FLAG_68000, &opcode_423 };
static opcode opcode_426 = { SIZE_L, CONTROL_REG, NUM_AREG, 0x4E7A, 0x8000, 0, APPEND, AUX(12), FLAG_68010, NULL };
static opcode opcode_427 = { SIZE_L, NUM_DREG, CONTROL_REG, 0x4E7B, 0x0000, 0, AUX(12), APPEND, FLAG_68010, &opcode_426 };
static opcode opcode_428 = { SIZE_L, NUM_AREG, CONTROL_REG, 0x4E7B, 0x8000, 0, AUX(12), APPEND, FLAG_68010, &opcode_427 };
static opcode opcode_425 = { SIZE_L, CONTROL_REG, NUM_DREG, 0x4E7A, 0x0000, 0, APPEND, AUX(12), FLAG_68010, &opcode_428 };
static opcode opcode_271 = { SIZE_WL, EA_SOURCE_ARG, NUM_AREG, 0x2040, 0x0000, 12, 0, 9, FLAG_68000|FLAG_COMP_SIZE, NULL };
static opcode opcode_260 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, NUM_DREG, 0xE008, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_261 = { SIZE_W, EA_MEMORY_ARG, NO_ARGUMENT, 0xE2C0, 0x0000, 0, 0, 0, FLAG_68000, &opcode_260 };
static opcode opcode_259 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xE028, 0x0000, 6, 9, 0, FLAG_68000, &opcode_261 };
static opcode opcode_285 = { SIZE_WL, EA_AREG_IND_DISP, NUM_DREG, 0x0108, 0x0000, 6, 0, 9, FLAG_68000, NULL };
static opcode opcode_284 = { SIZE_WL, NUM_DREG, EA_AREG_IND_DISP, 0x0188, 0x0000, 6, 9, 0, FLAG_68000, &opcode_285 };
static opcode opcode_431 = { SIZE_BWL, EA_MEMORY_ARG, NUM_AREG, 0x0E00, 0x8000, 6, 0, AUX(12), FLAG_68010, NULL };
static opcode opcode_432 = { SIZE_BWL, NUM_DREG, EA_MEMORY_ARG, 0x0E00, 0x0800, 6, AUX(12), 0, FLAG_68010, &opcode_431 };
static opcode opcode_433 = { SIZE_BWL, NUM_AREG, EA_MEMORY_ARG, 0x0E00, 0x8800, 6, AUX(12), 0, FLAG_68010, &opcode_432 };
static opcode opcode_430 = { SIZE_BWL, EA_MEMORY_ARG, NUM_DREG, 0x0E00, 0x0000, 6, 0, AUX(12), FLAG_68010, &opcode_433 };
static opcode opcode_289 = { SIZE_L, NUM_IMMEDIATE_8, NUM_DREG, 0x7000, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_294 = { SIZE_W, EA_SOURCE_ARG, NUM_DREG, 0xC0C0, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_302 = { SIZE_BWL, EA_DEST_ARG, NO_ARGUMENT, 0x4400, 0x0000, 6, 0, 0, FLAG_68000, NULL };
static opcode opcode_298 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x4800, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_293 = { SIZE_W, EA_SOURCE_ARG, NUM_DREG, 0xC1C0, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_279 = { SIZE_WL, REGISTER_LIST, EA_AREG_DEC_IND, 0x4880, 0x0000, 6, APPEND, 0, FLAG_68000|FLAG_REV_ARG_1, NULL };
static opcode opcode_280 = { SIZE_WL, EA_MOVEM_ARG|EA_AREG_IND_INC, REGISTER_LIST, 0x4C80, 0x0000, 6, 0, APPEND, FLAG_68000, &opcode_279 };
static opcode opcode_278 = { SIZE_WL, REGISTER_LIST, EA_MOVEM_ARG, 0x4880, 0x0000, 6, APPEND, 0, FLAG_68000, &opcode_280 };
static opcode opcode_307 = { NO_SIZE, NO_ARGUMENT, NO_ARGUMENT, 0x4E71, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_316 = { SIZE_BWL, EA_SOURCE_ARG, NUM_DREG, 0x8000, 0x0000, 6, 0, 9, FLAG_68000, NULL };
static opcode opcode_315 = { SIZE_BWL, NUM_DREG, EA_MEMORY_ARG, 0x8100, 0x0000, 6, 9, 0, FLAG_68000, &opcode_316 };
static opcode opcode_311 = { SIZE_BWL, EA_DEST_ARG, NO_ARGUMENT, 0x4600, 0x0000, 6, 0, 0, FLAG_68000, NULL };
static opcode opcode_321 = { NO_SIZE, EA_ADDRESS_ARG, NO_ARGUMENT, 0x4840, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_330 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, NUM_DREG, 0xE118, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_331 = { SIZE_W, EA_MEMORY_ARG, NO_ARGUMENT, 0xE7C0, 0x0000, 0, 0, 0, FLAG_68000, &opcode_330 };
static opcode opcode_329 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xE138, 0x0000, 6, 9, 0, FLAG_68000, &opcode_331 };
static opcode opcode_325 = { NO_SIZE, NO_ARGUMENT, NO_ARGUMENT, 0x4E70, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_317 = { SIZE_BWL, EA_IMMEDIATE, EA_DEST_ARG, 0x0000, 0x0000, 6, APPEND, 0, FLAG_68000, NULL };
static opcode opcode_339 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, NUM_DREG, 0xE110, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_340 = { SIZE_W, EA_MEMORY_ARG, NO_ARGUMENT, 0xE5C0, 0x0000, 0, 0, 0, FLAG_68000, &opcode_339 };
static opcode opcode_338 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xE130, 0x0000, 6, 9, 0, FLAG_68000, &opcode_340 };
static opcode opcode_437 = { NO_SIZE, NUM_IMMEDIATE_16, NO_ARGUMENT, 0x4E74, 0x0000, 0, APPEND, 0, FLAG_68010, NULL };
static opcode opcode_342 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, NUM_DREG, 0xE010, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_343 = { SIZE_W, EA_MEMORY_ARG, NO_ARGUMENT, 0xE4C0, 0x0000, 0, 0, 0, FLAG_68000, &opcode_342 };
static opcode opcode_341 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xE030, 0x0000, 6, 9, 0, FLAG_68000, &opcode_343 };
static opcode opcode_348 = { NO_SIZE, NO_ARGUMENT, NO_ARGUMENT, 0x4E77, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_354 = { SIZE_B, NUM_AREG_DEC_IND, NUM_AREG_DEC_IND, 0x8108, 0x0000, 0, 0, 9, FLAG_68000, NULL };
static opcode opcode_353 = { SIZE_B, NUM_DREG, NUM_DREG, 0x8100, 0x0000, 0, 0, 9, FLAG_68000, &opcode_354 };
static opcode opcode_349 = { NO_SIZE, NO_ARGUMENT, NO_ARGUMENT, 0x4E75, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_347 = { NO_SIZE, NO_ARGUMENT, NO_ARGUMENT, 0x4E73, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_333 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, NUM_DREG, 0xE018, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_334 = { SIZE_W, EA_MEMORY_ARG, NO_ARGUMENT, 0xE6C0, 0x0000, 0, 0, 0, FLAG_68000, &opcode_333 };
static opcode opcode_332 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0xE038, 0x0000, 6, 9, 0, FLAG_68000, &opcode_334 };
static opcode opcode_303 = { SIZE_BWL, EA_DEST_ARG, NO_ARGUMENT, 0x4000, 0x0000, 6, 0, 0, FLAG_68000, NULL };
static opcode opcode_368 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x55C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_363 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x51C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_371 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x57C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_378 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x5EC0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_367 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x54C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_364 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x52C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_376 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x5CC0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_369 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x55C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_377 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x5DC0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_365 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x53C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_370 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x56C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_362 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x50C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_374 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x5AC0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_375 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x5BC0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_379 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x5FC0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_388 = { SIZE_BWL, EA_SOURCE_ARG, NUM_DREG, 0x9000, 0x0000, 6, 0, 9, FLAG_68000, NULL };
static opcode opcode_387 = { SIZE_BWL, NUM_DREG, EA_MEMORY_ARG, 0x9100, 0x0000, 6, 9, 0, FLAG_68000, &opcode_388 };
static opcode opcode_390 = { SIZE_BWL, EA_IMMEDIATE, EA_DEST_ARG, 0x0400, 0x0000, 6, APPEND, 0, FLAG_68000, NULL };
static opcode opcode_389 = { SIZE_WL, EA_SOURCE_ARG, NUM_AREG, 0x90C0, 0x0000, 8, 0, 9, FLAG_68000, NULL };
static opcode opcode_393 = { SIZE_BWL, NUM_AREG_DEC_IND, NUM_AREG_DEC_IND, 0x9108, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_392 = { SIZE_BWL, NUM_DREG, NUM_DREG, 0x9100, 0x0000, 6, 9, 0, FLAG_68000, &opcode_393 };
static opcode opcode_373 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x59C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_372 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x58C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_391 = { SIZE_BWL, NUM_IMMEDIATE_QUICK, EA_DEST_ARG|EA_AREG, 0x5100, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_401 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x4AC0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_409 = { NO_SIZE, NO_ARGUMENT, NO_ARGUMENT, 0x4E76, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_405 = { NO_SIZE, NUM_IMMEDIATE_4, NO_ARGUMENT, 0x4E40, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_417 = { NO_SIZE, NUM_AREG, NO_ARGUMENT, 0x4E58, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_225 = { SIZE_BWL, EA_IMMEDIATE, EA_DEST_ARG, 0x0A00, 0x0000, 6, APPEND, 0, FLAG_68000, NULL };
static opcode opcode_223 = { SIZE_BWL, NUM_DREG, EA_DEST_ARG, 0xB100, 0x0000, 6, 9, 0, FLAG_68000, NULL };
static opcode opcode_413 = { SIZE_BWL, EA_DEST_ARG, NO_ARGUMENT, 0x4A00, 0x0000, 6, 0, 0, FLAG_68000, NULL };
static opcode opcode_397 = { SIZE_W, NUM_DREG, NO_ARGUMENT, 0x4840, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_383 = { NO_SIZE, NO_ARGUMENT, NO_ARGUMENT, 0x4E72, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_366 = { SIZE_B, EA_DEST_ARG, NO_ARGUMENT, 0x54C0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static opcode opcode_243 = { NO_SIZE, EA_ADDRESS_ARG, NO_ARGUMENT, 0x4EC0, 0x0000, 0, 0, 0, FLAG_68000, NULL };
static mnemonic mnemonic_103 = { "ABCD", &opcode_103, NULL, NULL };
static mnemonic mnemonic_108 = { "ADD", &opcode_108, &mnemonic_103, NULL };
static mnemonic mnemonic_111 = { "ADDI", &opcode_111, NULL, NULL };
static mnemonic mnemonic_110 = { "ADDA", &opcode_110, &mnemonic_108, &mnemonic_111 };
static mnemonic mnemonic_113 = { "ADDX", &opcode_113, NULL, NULL };
static mnemonic mnemonic_120 = { "ANDI", &opcode_120, NULL, NULL };
static mnemonic mnemonic_118 = { "AND", &opcode_118, &mnemonic_113, &mnemonic_120 };
static mnemonic mnemonic_112 = { "ADDQ", &opcode_112, &mnemonic_110, &mnemonic_118 };
static mnemonic mnemonic_127 = { "ASR", &opcode_127, NULL, NULL };
static mnemonic mnemonic_159 = { "BCHG", &opcode_159, NULL, NULL };
static mnemonic mnemonic_142 = { "BCC", &opcode_142, &mnemonic_127, &mnemonic_159 };
static mnemonic mnemonic_144 = { "BCS", &opcode_144, NULL, NULL };
static mnemonic mnemonic_152 = { "BGE", &opcode_152, NULL, NULL };
static mnemonic mnemonic_147 = { "BEQ", &opcode_147, &mnemonic_144, &mnemonic_152 };
static mnemonic mnemonic_164 = { "BCLR", &opcode_164, &mnemonic_142, &mnemonic_147 };
static mnemonic mnemonic_124 = { "ASL", &opcode_124, &mnemonic_112, &mnemonic_164 };
static mnemonic mnemonic_140 = { "BHI", &opcode_140, NULL, NULL };
static mnemonic mnemonic_155 = { "BLE", &opcode_155, NULL, NULL };
static mnemonic mnemonic_143 = { "BHS", &opcode_143, &mnemonic_140, &mnemonic_155 };
static mnemonic mnemonic_141 = { "BLS", &opcode_141, NULL, NULL };
static mnemonic mnemonic_151 = { "BMI", &opcode_151, NULL, NULL };
static mnemonic mnemonic_153 = { "BLT", &opcode_153, &mnemonic_141, &mnemonic_151 };
static mnemonic mnemonic_145 = { "BLO", &opcode_145, &mnemonic_143, &mnemonic_153 };
static mnemonic mnemonic_150 = { "BPL", &opcode_150, NULL, NULL };
static mnemonic mnemonic_169 = { "BSET", &opcode_169, NULL, NULL };
static mnemonic mnemonic_138 = { "BRA", &opcode_138, &mnemonic_150, &mnemonic_169 };
static mnemonic mnemonic_174 = { "BTST", &opcode_174, NULL, NULL };
static mnemonic mnemonic_149 = { "BVS", &opcode_149, NULL, NULL };
static mnemonic mnemonic_148 = { "BVC", &opcode_148, &mnemonic_174, &mnemonic_149 };
static mnemonic mnemonic_139 = { "BSR", &opcode_139, &mnemonic_138, &mnemonic_148 };
static mnemonic mnemonic_146 = { "BNE", &opcode_146, &mnemonic_145, &mnemonic_139 };
static mnemonic mnemonic_154 = { "BGT", &opcode_154, &mnemonic_124, &mnemonic_146 };
static mnemonic mnemonic_181 = { "CLR", &opcode_181, NULL, NULL };
static mnemonic mnemonic_184 = { "CMPA", &opcode_184, NULL, NULL };
static mnemonic mnemonic_183 = { "CMP", &opcode_183, &mnemonic_181, &mnemonic_184 };
static mnemonic mnemonic_186 = { "CMPM", &opcode_186, NULL, NULL };
static mnemonic mnemonic_202 = { "DBCS", &opcode_202, NULL, NULL };
static mnemonic mnemonic_200 = { "DBCC", &opcode_200, &mnemonic_186, &mnemonic_202 };
static mnemonic mnemonic_185 = { "CMPI", &opcode_185, &mnemonic_183, &mnemonic_200 };
static mnemonic mnemonic_196 = { "DBF", &opcode_196, NULL, NULL };
static mnemonic mnemonic_212 = { "DBGT", &opcode_212, NULL, NULL };
static mnemonic mnemonic_210 = { "DBGE", &opcode_210, &mnemonic_196, &mnemonic_212 };
static mnemonic mnemonic_201 = { "DBHS", &opcode_201, NULL, NULL };
static mnemonic mnemonic_203 = { "DBLO", &opcode_203, NULL, NULL };
static mnemonic mnemonic_213 = { "DBLE", &opcode_213, &mnemonic_201, &mnemonic_203 };
static mnemonic mnemonic_198 = { "DBHI", &opcode_198, &mnemonic_210, &mnemonic_213 };
static mnemonic mnemonic_205 = { "DBEQ", &opcode_205, &mnemonic_185, &mnemonic_198 };
static mnemonic mnemonic_211 = { "DBLT", &opcode_211, NULL, NULL };
static mnemonic mnemonic_204 = { "DBNE", &opcode_204, NULL, NULL };
static mnemonic mnemonic_209 = { "DBMI", &opcode_209, &mnemonic_211, &mnemonic_204 };
static mnemonic mnemonic_197 = { "DBRA", &opcode_197, NULL, NULL };
static mnemonic mnemonic_206 = { "DBVC", &opcode_206, NULL, NULL };
static mnemonic mnemonic_195 = { "DBT", &opcode_195, &mnemonic_197, &mnemonic_206 };
static mnemonic mnemonic_208 = { "DBPL", &opcode_208, &mnemonic_209, &mnemonic_195 };
static mnemonic mnemonic_217 = { "DIVS", &opcode_217, NULL, NULL };
static mnemonic mnemonic_222 = { "EOR", &opcode_222, NULL, NULL };
static mnemonic mnemonic_218 = { "DIVU", &opcode_218, &mnemonic_217, &mnemonic_222 };
static mnemonic mnemonic_229 = { "EXG", &opcode_229, NULL, NULL };
static mnemonic mnemonic_239 = { "ILLEGAL", &opcode_239, NULL, NULL };
static mnemonic mnemonic_235 = { "EXT", &opcode_235, &mnemonic_229, &mnemonic_239 };
static mnemonic mnemonic_224 = { "EORI", &opcode_224, &mnemonic_218, &mnemonic_235 };
static mnemonic mnemonic_207 = { "DBVS", &opcode_207, &mnemonic_208, &mnemonic_224 };
static mnemonic mnemonic_199 = { "DBLS", &opcode_199, &mnemonic_205, &mnemonic_207 };
static mnemonic mnemonic_179 = { "CHK", &opcode_179, &mnemonic_154, &mnemonic_199 };
static mnemonic mnemonic_244 = { "JSR", &opcode_244, NULL, NULL };
static mnemonic mnemonic_248 = { "LEA", &opcode_248, &mnemonic_244, NULL };
static mnemonic mnemonic_256 = { "LSL", &opcode_256, NULL, NULL };
static mnemonic mnemonic_252 = { "LINK", &opcode_252, &mnemonic_248, &mnemonic_256 };
static mnemonic mnemonic_265 = { "MOVE", &opcode_265, NULL, NULL };
static mnemonic mnemonic_425 = { "MOVEC", &opcode_425, NULL, NULL };
static mnemonic mnemonic_271 = { "MOVEA", &opcode_271, &mnemonic_265, &mnemonic_425 };
static mnemonic mnemonic_259 = { "LSR", &opcode_259, &mnemonic_252, &mnemonic_271 };
static mnemonic mnemonic_284 = { "MOVEP", &opcode_284, NULL, NULL };
static mnemonic mnemonic_430 = { "MOVES", &opcode_430, NULL, NULL };
static mnemonic mnemonic_289 = { "MOVEQ", &opcode_289, &mnemonic_284, &mnemonic_430 };
static mnemonic mnemonic_294 = { "MULU", &opcode_294, NULL, NULL };
static mnemonic mnemonic_302 = { "NEG", &opcode_302, NULL, NULL };
static mnemonic mnemonic_298 = { "NBCD", &opcode_298, &mnemonic_294, &mnemonic_302 };
static mnemonic mnemonic_293 = { "MULS", &opcode_293, &mnemonic_289, &mnemonic_298 };
static mnemonic mnemonic_278 = { "MOVEM", &opcode_278, &mnemonic_259, &mnemonic_293 };
static mnemonic mnemonic_307 = { "NOP", &opcode_307, NULL, NULL };
static mnemonic mnemonic_315 = { "OR", &opcode_315, NULL, NULL };
static mnemonic mnemonic_311 = { "NOT", &opcode_311, &mnemonic_307, &mnemonic_315 };
static mnemonic mnemonic_321 = { "PEA", &opcode_321, NULL, NULL };
static mnemonic mnemonic_329 = { "ROL", &opcode_329, NULL, NULL };
static mnemonic mnemonic_325 = { "RESET", &opcode_325, &mnemonic_321, &mnemonic_329 };
static mnemonic mnemonic_317 = { "ORI", &opcode_317, &mnemonic_311, &mnemonic_325 };
static mnemonic mnemonic_338 = { "ROXL", &opcode_338, NULL, NULL };
static mnemonic mnemonic_437 = { "RTD", &opcode_437, NULL, NULL };
static mnemonic mnemonic_341 = { "ROXR", &opcode_341, &mnemonic_338, &mnemonic_437 };
static mnemonic mnemonic_348 = { "RTR", &opcode_348, NULL, NULL };
static mnemonic mnemonic_353 = { "SBCD", &opcode_353, NULL, NULL };
static mnemonic mnemonic_349 = { "RTS", &opcode_349, &mnemonic_348, &mnemonic_353 };
static mnemonic mnemonic_347 = { "RTE", &opcode_347, &mnemonic_341, &mnemonic_349 };
static mnemonic mnemonic_332 = { "ROR", &opcode_332, &mnemonic_317, &mnemonic_347 };
static mnemonic mnemonic_303 = { "NEGX", &opcode_303, &mnemonic_278, &mnemonic_332 };
static mnemonic mnemonic_368 = { "SCS", &opcode_368, NULL, NULL };
static mnemonic mnemonic_363 = { "SF", &opcode_363, NULL, NULL };
static mnemonic mnemonic_371 = { "SEQ", &opcode_371, &mnemonic_368, &mnemonic_363 };
static mnemonic mnemonic_378 = { "SGT", &opcode_378, NULL, NULL };
static mnemonic mnemonic_367 = { "SHS", &opcode_367, NULL, NULL };
static mnemonic mnemonic_364 = { "SHI", &opcode_364, &mnemonic_378, &mnemonic_367 };
static mnemonic mnemonic_376 = { "SGE", &opcode_376, &mnemonic_371, &mnemonic_364 };
static mnemonic mnemonic_369 = { "SLO", &opcode_369, NULL, NULL };
static mnemonic mnemonic_377 = { "SLT", &opcode_377, NULL, NULL };
static mnemonic mnemonic_365 = { "SLS", &opcode_365, &mnemonic_369, &mnemonic_377 };
static mnemonic mnemonic_370 = { "SNE", &opcode_370, NULL, NULL };
static mnemonic mnemonic_362 = { "ST", &opcode_362, NULL, NULL };
static mnemonic mnemonic_374 = { "SPL", &opcode_374, &mnemonic_370, &mnemonic_362 };
static mnemonic mnemonic_375 = { "SMI", &opcode_375, &mnemonic_365, &mnemonic_374 };
static mnemonic mnemonic_379 = { "SLE", &opcode_379, &mnemonic_376, &mnemonic_375 };
static mnemonic mnemonic_387 = { "SUB", &opcode_387, NULL, NULL };
static mnemonic mnemonic_390 = { "SUBI", &opcode_390, NULL, NULL };
static mnemonic mnemonic_389 = { "SUBA", &opcode_389, &mnemonic_387, &mnemonic_390 };
static mnemonic mnemonic_392 = { "SUBX", &opcode_392, NULL, NULL };
static mnemonic mnemonic_373 = { "SVS", &opcode_373, NULL, NULL };
static mnemonic mnemonic_372 = { "SVC", &opcode_372, &mnemonic_392, &mnemonic_373 };
static mnemonic mnemonic_391 = { "SUBQ", &opcode_391, &mnemonic_389, &mnemonic_372 };
static mnemonic mnemonic_401 = { "TAS", &opcode_401, NULL, NULL };
static mnemonic mnemonic_409 = { "TRAPV", &opcode_409, NULL, NULL };
static mnemonic mnemonic_405 = { "TRAP", &opcode_405, &mnemonic_401, &mnemonic_409 };
static mnemonic mnemonic_417 = { "UNLK", &opcode_417, NULL, NULL };
static mnemonic mnemonic_225 = { "XORI", &opcode_225, NULL, NULL };
static mnemonic mnemonic_223 = { "XOR", &opcode_223, &mnemonic_417, &mnemonic_225 };
static mnemonic mnemonic_413 = { "TST", &opcode_413, &mnemonic_405, &mnemonic_223 };
static mnemonic mnemonic_397 = { "SWAP", &opcode_397, &mnemonic_391, &mnemonic_413 };
static mnemonic mnemonic_383 = { "STOP", &opcode_383, &mnemonic_379, &mnemonic_397 };
static mnemonic mnemonic_366 = { "SCC", &opcode_366, &mnemonic_303, &mnemonic_383 };
static mnemonic mnemonic_243 = { "JMP", &opcode_243, &mnemonic_179, &mnemonic_366 };
#define ROOT_NODE mnemonic_243
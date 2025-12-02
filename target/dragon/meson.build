gen = [
    decodetree.process('insn.decode', extra_args: [
        '--decode', 'decode_insn', '--insnwidth', '32'
    ])
]

dragon_ss = ss.source_set()
dragon_softmmu_ss = ss.source_set()

dragon_ss.add(gen)
dragon_ss.add(files(
    'cpu.c',
    'disas.c',
    'helper.c',
    'translate.c',
))
dragon_softmmu_ss.add(files(

))

target_arch += {'dragon': dragon_ss}
target_system_arch += {'dragon': dragon_softmmu_ss}
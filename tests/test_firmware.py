"""Actual ARM C parser and GPIO dispatch, emulated with HAL GPIO test doubles."""
from pathlib import Path
import subprocess, json, itertools
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS
from unicorn.arm_const import UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_R0
ROOT=Path(__file__).resolve().parents[1]
P=ROOT/'Rover_UART_GPIO'
tools=next(Path('C:/ST/STM32CubeIDE_2.2.0/STM32CubeIDE/plugins').glob('*gnu-tools*'))/'tools/bin'
B=ROOT/'build/tests'; B.mkdir(parents=True,exist_ok=True)
flags=['-mcpu=cortex-m4','-mthumb','-O1','-ffunction-sections','-fdata-sections','-DSTM32F446xx','-DUSE_HAL_DRIVER']
incs=['Core/Inc','Drivers/CMSIS/Include','Drivers/CMSIS/Device/ST/STM32F4xx/Include','Drivers/STM32F4xx_HAL_Driver/Inc']
cmd=[str(tools/'arm-none-eabi-gcc.exe'),*flags,*['-I'+str(P/i) for i in incs],'-nostdlib',str(ROOT/'tests/harness.c'),str(P/'Core/Src/command.c'),'-Wl,-Ttext=0x1000,--gc-sections,-e,Test_Reset,-u,Test_Byte,-u,Test_State','-o',str(B/'test.elf')]
subprocess.run(cmd,check=True,capture_output=True)
subprocess.run([str(tools/'arm-none-eabi-objcopy.exe'),'-O','binary',str(B/'test.elf'),str(B/'test.bin')],check=True)
nm=subprocess.check_output([str(tools/'arm-none-eabi-nm.exe'),str(B/'test.elf')],text=True)
symbols={line.split()[2]:int(line.split()[0],16) for line in nm.splitlines() if len(line.split())==3}
uc=Uc(UC_ARCH_ARM,UC_MODE_THUMB|UC_MODE_MCLASS); uc.mem_map(0x1000,0x40000)
uc.mem_write(0x1000,(B/'test.bin').read_bytes())
def call(name,arg=0):
    uc.reg_write(UC_ARM_REG_SP,0x3f000); uc.reg_write(UC_ARM_REG_LR,0x3e001); uc.reg_write(UC_ARM_REG_R0,arg)
    uc.emu_start(symbols[name]|1,0x3e000,count=10000)
    return uc.reg_read(UC_ARM_REG_R0)
results=[]
def run(label,initial,message,expected,accepted):
    call('Test_Reset',initial)
    count=sum(call('Test_Byte',byte) for byte in message.encode('ascii'))
    actual=call('Test_State')
    assert (actual,count)==(expected,accepted),(label,actual,count,expected,accepted)
    results.append({'case':label,'initial':initial,'message':repr(message),'expected':expected,'actual':actual,'accepted':count,'result':'PASS'})
for initial in range(4):
    for a,b in itertools.product('012',repeat=2):
        expected=initial
        for i,char in enumerate((a,b)):
            mask=1<<i
            if char=='0': expected &= ~mask
            elif char=='1': expected |= mask
            else: expected ^= mask
        run(f'command_{a}{b}_initial_{initial}',initial,a+b+'\r\n',expected,1)
for initial in range(4):
    for msg in ['','\r\n','1\n','111\n','2x\n','x21\n',' 11\n','11 \n','99\n','11','1'*100+'\n']:
        run('invalid_or_incomplete',initial,msg,initial,0)
run('recovery_after_invalid',0,'x21\n11\n',3,1)
run('toggle_twice',0,'22\n22\n',0,2)
run('mixed_sequence',0,'11\n10\n01\n00\n21\n',3,5)
run('empty_lines_do_not_repeat_toggle',0,'22\r\n\r\n',3,1)
run('CR_only',0,'11\r',3,1)
run('LF_only',0,'11\n',3,1)
(ROOT/'test_results.json').write_text(json.dumps({'method':'ARM Cortex-M4 emulation of real command.c and ApplyPin; HAL GPIO mocked; not hardware test','passed':len(results),'cases':results},ensure_ascii=False,indent=2),encoding='utf-8')
print(f'{len(results)} ARM firmware cases PASS; hardware not tested')

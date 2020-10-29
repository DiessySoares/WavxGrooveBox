# WavxGrooveBox
 Drum Machine randomica feita em C.

## O que falta:
- Parametrização geral para controlar o comportamento (provavelmente por meio de argumentos).
- Interface simples baseada em texto.

## O que ja esta implementado:
- Carregamento das bibliotecas "rom" e "pat"
- Randomização dos parametros de escolha
- Criacao de "novos" samples baseados nos existestes
- Criacao dew novos padroes randomicos.
- Sequencimanto
- 8 Efeito de audio
- Audio final em ".wav".

**Efeitos:**
Nome | Descricao 
-- | ---- 
pitch | controla o tom do sample  
distortion | Amplia o som ate certo limite, causando um efeito de distorcao
lowpass | filtro que remove frequencias mais altas ou agudas    
flanger | dois audio identicos interpolados com uma variação pequena de tempo, variacao controlada pela funcao cos() que altera a velocidade de reproducao de um dos audios por meio de uma sinewave.
wow | Pequena alteracao no tempo execução, dando o efeito de toca fitas antigo ou vinyl
decimator | um efeito que reduz a resolução da waveform, simulando a limitacao que sistemas antigos tinham nativamente
sidechain | controle de volume que se baiseia em outro audio como parametro, no caso do projeto ele se baiseia no padrao de execucao, diminuido o volume do audio onde outro audio pode ter espaco.

## O que pode ou nao existir:
- highpass filter, filtro que retira frequecias baixa ou graves, baseado no cancelamento de fase entre um audio e o mesmo audio com lowpass filter. (nao sei se da certo)
- *Aphex Twin* mode
- Novas bibliotecas
- Alguma coisa que me vier a cabeca
---
**(A pasta output é onde estara os audios gerados, a pasta precisa existir.)**


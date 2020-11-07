# WavxGrooveBox
 Drum Machine randomica feita em C.

## O que falta:
- Algumas coisas, bugs e coisas do tipo

## Modo de uso *(por etapa)*
- Biblioteca
*Arquivo onde ficam os arquivo de audio, cada uma delas tem elementos difererente para diferentres estilos*
- Padrões
*Arquivo onde se encontra algunas padrões ritimicos, assim como as bilbliotecas*

**Os padrões e bibliotecas nao precisam ser iguais, podem ser combinados livremente**

- Gerar
*Opção que cria arquivo de audio*

- Alterar hash
*Alterar a hash influencia no resultado final, cada hash da um resultado diferente, e uma hash sempre da um resultado igual (ou era pra ser assim)*

- Alterar randomicamente a hash
*Semelhante a descrição de cima, mas a hash é gerada por uma funcao randomica*

- Mudar configuracoes
*Apresenta algumas configurações basicas para o resultado final*

- Mudar configuracoes de efeitos
*Uma lista de efeitos que serão aplicados ao resultado final, a descrições deles se apresenta na tabela abaixo*

**Efeitos:**
Nome | Descricao 
-- | ---- 
volume | amplitude total do resultado final  
pitch | controla o tom do sample  
distortion | Amplia o som ate certo limite, causando um efeito de distorcao
lowpass | filtro que remove frequencias mais altas ou agudas    
flanger | dois audio identicos interpolados com uma variação pequena de tempo, variacao controlada pela funcao cos() que altera a velocidade de reproducao de um dos audios por meio de uma sinewave.
decimator | um efeito que reduz a resolução da waveform, simulando a limitacao que sistemas antigos tinham nativamente


## O que pode ou nao existir:
- highpass filter, filtro que retira frequecias baixa ou graves, baseado no cancelamento de fase entre um audio e o mesmo audio com lowpass filter. (nao sei se da certo)
- *Aphex Twin* mode
- Novas bibliotecas
- Alguma coisa que me vier a cabeca
---
**(A pasta output é onde estara os audios gerados, a pasta precisa existir.)**


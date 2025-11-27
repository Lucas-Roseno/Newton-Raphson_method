# Implementação do Método de Newton-Raphson em C++

Este projeto consiste na implementação computacional do **Método de Newton-Raphson**, um algoritmo iterativo utilizado para encontrar aproximações de raízes de funções reais não lineares.

O software foi desenvolvido como parte da disciplina de **Métodos Numéricos e Computacionais**, atendendo aos requisitos de flexibilidade de entrada, precisão numérica e exportação de dados.

## 🚀 Funcionalidades

* **Alta Precisão:** Utiliza tipos de dados `long double` (precisão estendida) para minimizar erros de arredondamento em iterações avançadas.
* **Derivada Automática (NOVO):** O usuário não precisa saber calcular a derivada de $f(x)$. O programa pode calculá-la numericamente usando o Método das Diferenças Finitas.
* **Interpretador de Expressões (Parser):** Permite que o usuário digite sua própria função e derivada manualmente (ex: `x^2 - 4*cos(x)`) sem necessidade de recompilar o código.
* **Correção de Sintaxe:** O sistema identifica e corrige automaticamente multiplicações implícitas (ex: converte `2x` para `2*x`).
* **Exportação de Dados:** Gera automaticamente um arquivo `tabela.csv` ao final da execução, contendo o histórico detalhado das iterações para análise em planilhas (Excel/Google Sheets).
* **Proteção de Erros:** Tratamento de exceções para divisões por zero (derivada nula) e erros de sintaxe matemática.

## 🛠️ Pré-requisitos

Para compilar e executar este projeto, você precisará de:
* Um compilador C++ moderno (GCC/G++, Clang ou MSVC) que suporte o padrão C++11 ou superior.

## 💻 Compilação e Execução

Abra o terminal na pasta do arquivo `main.cpp` e execute os comandos abaixo dependendo do seu sistema operacional.

### Linux / macOS

1.  **Compilar:**
    ```bash
    g++ main.cpp -o newton_raphson
    ```
2.  **Executar:**
    ```bash
    ./newton_raphson
    ```

### Windows

1.  **Compilar:**
    ```cmd
    g++ main.cpp -o newton_raphson.exe
    ```
2.  **Executar:**
    ```cmd
    newton_raphson.exe
    ```

---

## 📝 Como Inserir Funções (Guia de Sintaxe)

Ao selecionar a **Opção 5 (DIGITAR MINHA PRÓPRIA FUNÇÃO)** no menu, você deve seguir a sintaxe matemática padrão.

### Operadores Básicos
| Operação | Símbolo | Exemplo |
| :--- | :---: | :--- |
| Adição | `+` | `x + 2` |
| Subtração | `-` | `x - 5` |
| Multiplicação | `*` | `3*x` (ou `3x`) |
| Divisão | `/` | `x / 2` |
| Potenciação | `^` | `x^3` |

### Funções Matemáticas Suportadas
O parser reconhece as seguintes funções (sempre use parênteses):

* **Trigonometria:** `sin(x)`, `cos(x)`, `tan(x)`
* **Exponencial e Logaritmo:** `exp(x)` (para $e^x$), `log(x)` (logaritmo natural $\ln x$)
* **Raiz:** `sqrt(x)`

### Exemplos de Entrada
* **Polinômio:** `x^3 - 2x - 5`
* **Trigonométrica:** `cos(x) - x`
* **Exponencial:** `exp(-x) - x`
* **Complexa:** `x^2 - 4*sin(x) + sqrt(x)`

> **Nota:** O programa é capaz de entender multiplicação implícita entre números e variáveis.
> * Entrada do usuário: `2x^2`
> * Interpretação do sistema: `2*x^2`

---

## 📊 Resultados Esperados e Saída

O programa solicitará três parâmetros de parada/início:
1.  **Chute inicial ($x_0$):** O valor por onde o algoritmo deve começar a procurar.
2.  **Erro máximo ($\epsilon$):** A tolerância de precisão (ex: `0.0001`).
3.  **Máximo de iterações:** Um limite de segurança para evitar loops infinitos caso o método não convirja.

### Saída no Console
O programa exibirá uma tabela formatada em tempo real:

```text
ITERACOES (Newton-Raphson - Alta Precisao)
----------------------------------------------------------------------------------------------------------------------------------
k         x_n                                     f(x_n)                                  Erro Estimado                           
----------------------------------------------------------------------------------------------------------------------------------
1         10.000000000000                         96.000000000000                         5.234...
2         5.2000000000000                         ...                                     ...
...
RESULTADO FINAL:
Status: CONVERGIU COM SUCESSO
Raiz Aproximada: 2.094551481542
Total de Iteracoes: 5

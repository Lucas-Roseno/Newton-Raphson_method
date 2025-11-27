# Implementação do Método de Newton-Raphson

Este projeto consiste na implementação computacional do **Método de Newton-Raphson** em C++ para a disciplina de **Métodos Numéricos e Computacionais**. O software é capaz de encontrar raízes reais de funções não lineares através de um processo iterativo, permitindo tanto o uso de funções pré-definidas quanto a inserção de funções personalizadas pelo usuário com interpretação em tempo de execução.

## 👥 Autores

* **Deivy Rossi Teixeira de Melo**
* **Lucas Roseno Medeiros Araujo**
* **João Vitor Lobato Romualdo**

**Disciplina:** Métodos Numéricos e Computacionais
**Professor:** Erasmo Tales Fonseca
**Data:** Novembro de 2025

---

## 🚀 Funcionalidades

* **Menu Interativo:** Seleção simples entre funções de teste clássicas ou entrada personalizada.
* **Parser Matemático (SimpleParser):** Interpreta expressões matemáticas digitadas como texto (ex: `x^3 - 2*x - 5`) sem necessidade de recompilar o código.
* **Derivada Automática:** O usuário pode optar por deixar o programa calcular a derivada numericamente (Diferenças Finitas) ou inserir a derivada manual.
* **Exportação de Dados:** Gera automaticamente um arquivo `tabela.csv` com o histórico detalhado das iterações para análise em planilhas (Excel, Google Sheets).
* **Critérios de Parada:** Baseados em erro máximo permitido ($\epsilon$) ou número máximo de iterações.

---

## 🛠️ Pré-requisitos

Para compilar e executar este projeto, você precisará de um compilador C++ compatível com o padrão C++11 ou superior (ex: G++, Clang, MinGW).

## 💻 Como Compilar e Executar

1.  **Baixe o código:** Certifique-se de que o arquivo `main.cpp` está no diretório atual.
2.  **Compile:** Abra o terminal e execute o seguinte comando:
    ```bash
    g++ main.cpp -o newton_raphson
    ```
3.  **Execute:**
    * No Linux/Mac:
        ```bash
        ./newton_raphson
        ```
    * No Windows:
        ```cmd
        newton_raphson.exe
        ```

---

## 📖 Como Usar

Ao iniciar o programa, você verá um menu com 5 opções:

1.  **Funções Predefinidas (Opções 1 a 4):**
    Basta digitar o número correspondente à função desejada (ex: `1` para $f(x) = x^3 - 2x - 5$).

2.  **Função Personalizada (Opção 5):**
    * Escolha a opção `5`.
    * **Digite a função:** Escreva a expressão matemática usando `x` como variável.
    * **Derivada:** O programa perguntará: *"Deseja que o programa calcule a derivada automaticamente? (S/N)"*.
        * Digite `S` para cálculo automático (Diferenças Finitas).
        * Digite `N` para inserir a fórmula da derivada manualmente.

3.  **Parâmetros de Execução:**
    O programa solicitará os parâmetros numéricos:
    * `x0`: Chute inicial (valor estimado para começar a iteração).
    * `epsilon`: Tolerância de erro (ex: `1e-6` ou `0.000001`).
    * `maxIter`: Limite de iterações de segurança (para evitar loops infinitos).

### 📝 Sintaxe Suportada para Funções

O interpretador embutido (`SimpleParser`) suporta as seguintes operações e funções:

* **Operadores:** `+`, `-`, `*`, `/`, `^` (potência).
* **Funções Trigonométricas:** `sin(x)`, `cos(x)`, `tan(x)`.
* **Outras Funções:** `exp(x)` (exponencial $e^x$), `log(x)` (logaritmo natural), `sqrt(x)` (raiz quadrada).
* **Exemplos válidos:**
    * `x^3 - 2*x - 5`
    * `cos(x) - x`
    * `exp(-x) - x`

---

## 📊 Resultados Esperados

O programa exibirá no console uma tabela formatada contendo:
* **k:** Número da iteração.
* **x_n:** Aproximação atual da raiz.
* **f(x_n):** Valor da função no ponto atual.
* **Erro Estimado:** Diferença absoluta entre iterações consecutivas ($|x_{n} - x_{n-1}|$).

### Exemplo de Saída (Console)
```text
TABELA DE ITERACOES (Metodo de Newton-Raphson)
--------------------------------------------------
k         x_n             f(x_n)          Erro
--------------------------------------------------
1         2.100000...     0.046100...     0.1000...
2         2.094568...     0.000139...     0.0054...
...
RESULTADO FINAL:
Status: CONVERGIU COM SUCESSO
Raiz Aproximada: 2.094551481542
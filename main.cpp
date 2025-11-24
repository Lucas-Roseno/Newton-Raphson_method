/*
 * ATIVIDADE PRÁTICA 01 - MÉTODO DE NEWTON-RAPHSON
 * Disciplina: Métodos Numéricos e Computacionais
 * Descrição: Implementação do método iterativo para encontrar raízes de funções não lineares.
 * O programa aceita funções predefinidas ou inseridas manualmente pelo usuário,
 * calcula as iterações e exporta os resultados para um arquivo 'tabela.csv'.
 */

#include <iostream>
#include <cmath>      
#include <iomanip>  
#include <functional> 
#include <string>
#include <cctype>     
#include <stdexcept>
#include <fstream>   

using namespace std;

// ==========================================================
// CLASSE: SIMPLE PARSER (Interpretador de Expressões)
// Responsável por converter a string digitada (ex: "x^2 - 4") 
// em um valor numérico computável em tempo de execução.
// Utiliza a técnica de "Recursive Descent Parsing".
// ==========================================================
class SimpleParser {
private:
    string expr;       // A expressão em texto
    int pos;           // Posição atual do cursor na leitura da string
    long double x_val; // O valor atual de 'x' para substituir na equação

    // Verifica o caractere atual sem avançar o cursor
    char peek() {
        while (pos < expr.length() && isspace(expr[pos])) pos++;
        if (pos == expr.length()) return 0;
        return expr[pos];
    }

    // Lê e retorna o caractere atual, avançando o cursor
    char get() {
        char c = peek();
        if (pos < expr.length()) pos++;
        return c;
    }

    // Lê um número completo (inteiro ou decimal) da string
    long double parseNumber() { 
        char c = peek();
        string numStr;
        while (isdigit(c) || c == '.') {
            numStr += get();
            c = peek();
        }
        if (numStr.empty()) return 0.0L; 
        return stold(numStr); // Converte string para long double
    }

    // Processa fatores: números, parênteses, negativos e funções (sin, cos, etc.)
    long double parseFactor() {
        char c = peek();
        
        // Se for dígito, lê o número
        if (isdigit(c) || c == '.') return parseNumber();
        
        // Se for parêntese, resolve a expressão interna recursivamente
        if (c == '(') {
            get(); 
            long double result = parseExpression();
            if (get() != ')') throw runtime_error("Esperado ')'");
            return result;
        }
        
        // Operador unário negativo (ex: -5 ou -x)
        if (c == '-') {
            get();
            return -parseFactor();
        }

        // Variáveis e Funções Matemáticas
        if (isalpha(c)) {
            string name;
            while (isalpha(peek())) name += get();
            
            if (name == "x") return x_val;
            else if (name == "sin") return sin(parseFactor());
            else if (name == "cos") return cos(parseFactor());
            else if (name == "tan") return tan(parseFactor());
            else if (name == "exp") return exp(parseFactor()); // Exponencial e^x
            else if (name == "log") return log(parseFactor()); // Logaritmo natural
            else if (name == "sqrt") return sqrt(parseFactor());
            else throw runtime_error("Funcao desconhecida: " + name);
        }
        throw runtime_error("Caractere inesperado: " + string(1, c));
    }

    // Processa potências (^), que têm prioridade sobre multiplicação
    long double parsePower() {
        long double left = parseFactor();
        while (peek() == '^') {
            get();
            long double right = parseFactor();
            left = pow(left, right);
        }
        return left;
    }

    // Processa multiplicação e divisão
    long double parseTerm() {
        long double left = parsePower();
        while (peek() == '*' || peek() == '/') {
            char op = get();
            long double right = parsePower();
            if (op == '*') left *= right;
            else {
                // Proteção contra divisão por zero
                if (abs(right) < 1e-15L) throw runtime_error("Divisao por zero detectada na expressao");
                left /= right;
            }
        }
        return left;
    }

    // Processa soma e subtração (nível mais baixo de precedência)
    long double parseExpression() {
        long double left = parseTerm();
        while (peek() == '+' || peek() == '-') {
            char op = get();
            long double right = parseTerm();
            if (op == '+') left += right;
            else left -= right;
        }
        return left;
    }

public:
    // Método auxiliar para tratar erros comuns de digitação (ex: "2x" -> "2*x")
    string preprocess(string s) {
        string res = "";
        for (size_t i = 0; i < s.length(); i++) {
            res += s[i];
            // Se encontrar um número seguido imediatamente de uma letra, insere multiplicação
            if (i + 1 < s.length()) {
                if (isdigit(s[i]) && isalpha(s[i+1])) {
                    res += '*';
                }
            }
        }
        return res;
    }

    // Função principal chamada pelo código para calcular f(x)
    long double evaluate(string expression_str, long double x) {
        this->expr = preprocess(expression_str);
        this->pos = 0;
        this->x_val = x;
        return parseExpression();
    }
    
    // Retorna a string corrigida para exibição
    string getFixedString(string s) {
        return preprocess(s);
    }
};

// ==========================================================
// FUNÇÕES PREDEFINIDAS PARA TESTE
// Sugeridas no descritivo da atividade
// ==========================================================
long double func1(long double x) { return pow(x, 3) - 2 * x - 5; }
long double deriv1(long double x) { return 3 * pow(x, 2) - 2; }

long double func2(long double x) { return cos(x) - x; }
long double deriv2(long double x) { return -sin(x) - 1.0L; }

long double func3(long double x) { return exp(-x) - x; }
long double deriv3(long double x) { return -exp(-x) - 1.0L; }

long double func4(long double x) { return pow(x, 3) - 9 * x + 3; }
long double deriv4(long double x) { return 3 * pow(x, 2) - 9; }

// ==========================================================
// ALGORITMO: MÉTODO DE NEWTON-RAPHSON
// Recebe as funções f e f', o ponto inicial e os critérios de parada.
// ==========================================================
void newtonRaphson(
    std::function<long double(long double)> f,  // Função f(x)
    std::function<long double(long double)> df, // Derivada f'(x)
    long double x0,                             // Chute inicial
    long double epsilon,                        // Tolerância (Erro máximo)
    int maxIter                                 // Número máximo de iterações
) {
    long double x_atual = x0;
    long double x_prox;
    long double erro = epsilon + 1.0L; // Garante entrada no loop
    int iter = 0;

    // Prepara o arquivo para salvar os dados
    ofstream arquivoCsv;
    arquivoCsv.open("tabela.csv"); 
    
    // Cabeçalho do CSV
    arquivoCsv << "k,x_n,f(x_n),Erro Estimado" << endl;
    arquivoCsv << fixed << setprecision(15); 

    // Cabeçalho da Tabela no Console
    cout << "\n" << string(130, '-') << endl;
    cout << "TABELA DE ITERACOES (Metodo de Newton-Raphson)" << endl;
    cout << string(130, '-') << endl;
    // Formatação espaçada para acomodar números de alta precisão
    cout << left << setw(10) << "k" 
         << left << setw(40) << "x_n" 
         << left << setw(40) << "f(x_n)" 
         << left << setw(40) << "Erro Estimado" << endl;
    cout << string(130, '-') << endl;
    cout << fixed << setprecision(12);

    // Loop Iterativo Principal
    while (iter < maxIter) {
        long double fx, dfx;
        
        // Tratamento de erros de cálculo (ex: overflow, divisão por zero na função)
        try {
            fx = f(x_atual);
            dfx = df(x_atual);
        } catch (const exception& e) {
            cout << "\n[ERRO MATEMATICO DETECTADO] " << e.what() << endl;
            arquivoCsv << "ERRO," << e.what() << endl;
            arquivoCsv.close();
            return;
        }

        // Verificação Crítica: Derivada nula
        // O método falha se f'(x) = 0, pois causaria divisão por zero na fórmula
        if (abs(dfx) < 1e-18L) { 
            cout << "\n[ERRO] Derivada anulou-se em x = " << x_atual << ". O metodo nao pode prosseguir." << endl;
            arquivoCsv.close();
            return;
        }

        // --- FÓRMULA DO MÉTODO ---
        // x_{n+1} = x_n - f(x_n) / f'(x_n)
        x_prox = x_atual - (fx / dfx);

        // Cálculo do erro estimado: |x_{n+1} - x_n|
        erro = abs(x_prox - x_atual);

        // Exibição dos resultados na tela
        cout << left << setw(10) << iter + 1
             << left << setw(40) << x_atual
             << left << setw(40) << fx
             << left << setw(0) << erro << endl;

        // Gravação no arquivo
        arquivoCsv << (iter + 1) << "," 
                   << x_atual << "," 
                   << fx << "," 
                   << erro << endl;

        // Atualiza para a próxima iteração
        x_atual = x_prox;
        iter++;

        // CRITÉRIO DE PARADA 1: Convergência (Erro < Epsilon)
        if (erro < epsilon) {
            cout << string(130, '-') << endl;
            cout << "\nRESULTADO FINAL:" << endl;
            cout << "Status: CONVERGIU COM SUCESSO" << endl;
            cout << "Raiz Aproximada: " << x_atual << endl;
            cout << "Total de Iteracoes: " << iter << endl;
            
            cout << "\n[INFO] O arquivo 'tabela.csv' foi gerado com os dados detalhados." << endl;
            arquivoCsv.close(); 
            return;
        }
    }

    // CRITÉRIO DE PARADA 2: Número máximo de iterações atingido
    cout << string(130, '-') << endl;
    cout << "\nRESULTADO FINAL:" << endl;
    cout << "Status: NAO CONVERGIU (Limite de iteracoes atingido)" << endl;
    cout << "Ultima aproximacao: " << x_atual << endl;
    
    arquivoCsv.close();
}

// ==========================================================
// FUNÇÃO PRINCIPAL (Menu e Entradas)
// ==========================================================
int main() {
    int escolha;
    long double x0, epsilon;
    int maxIter;
    SimpleParser parser; // Instância do interpretador

    string userFuncStr, userDerivStr;
    
    // Ponteiros de função (std::function) para armazenar a escolha do usuário
    std::function<long double(long double)> f_selecionada;
    std::function<long double(long double)> df_selecionada;
    string desc_f;

    cout << "=== IMPLEMENTACAO: METODO DE NEWTON-RAPHSON ===" << endl;
    cout << "Escolha a funcao f(x) para encontrar a raiz:" << endl;
    cout << "1. f(x) = x^3 - 2x - 5" << endl;
    cout << "2. f(x) = cos(x) - x" << endl;
    cout << "3. f(x) = e^(-x) - x" << endl;
    cout << "4. f(x) = x^3 - 9x + 3" << endl;
    cout << "5. DIGITAR MINHA PROPRIA FUNCAO (Manual)" << endl;
    cout << "Opcao: ";
    cin >> escolha;

    if (escolha == 5) {
        // Modo de entrada manual da função
        cout << "\n--- GUIA DE SINTAXE ---" << endl;
        cout << "Operadores suportados: + - * / ^" << endl;
        cout << "Funcoes suportadas: sin, cos, tan, exp, log, sqrt" << endl;
        cout << "Exemplo: x^2 - 4*x + 1" << endl;
        cout << "------------------------------------" << endl;

        cout << "Digite a funcao f(x): ";
        cin.ignore(); // Limpeza de buffer necessária antes do getline
        getline(cin, userFuncStr);

        cout << "Digite a derivada f'(x): ";
        getline(cin, userDerivStr);

        // Exibe a interpretação para confirmação do usuário
        string fixedF = parser.getFixedString(userFuncStr);
        string fixedD = parser.getFixedString(userDerivStr);
        cout << "\n[Sistema] Funcao interpretada como:   " << fixedF << endl;
        cout << "[Sistema] Derivada interpretada como: " << fixedD << endl;

        desc_f = fixedF;

        // Configura as funções lambdas para usar o parser
        f_selecionada = [&parser, userFuncStr](long double x) {
            return parser.evaluate(userFuncStr, x);
        };
        df_selecionada = [&parser, userDerivStr](long double x) {
            return parser.evaluate(userDerivStr, x);
        };

    } else {
        // Seleção das funções hardcoded (compiladas)
        switch (escolha) {
            case 1: f_selecionada = func1; df_selecionada = deriv1; desc_f = "x^3 - 2x - 5"; break;
            case 2: f_selecionada = func2; df_selecionada = deriv2; desc_f = "cos(x) - x"; break;
            case 3: f_selecionada = func3; df_selecionada = deriv3; desc_f = "e^(-x) - x"; break;
            case 4: f_selecionada = func4; df_selecionada = deriv4; desc_f = "x^3 - 9x + 3"; break;
            default: cout << "Opcao invalida! Reinicie o programa." << endl; return 1;
        }
    }

    // Coleta dos parâmetros do método
    cout << "\n--- Parametros de Execucao ---" << endl;
    cout << "Digite o chute inicial (x0): ";
    cin >> x0;
    cout << "Digite o erro maximo permitido (epsilon): ";
    cin >> epsilon;
    cout << "Digite o numero maximo de iteracoes: ";
    cin >> maxIter;

    // Chamada da rotina principal
    newtonRaphson(f_selecionada, df_selecionada, x0, epsilon, maxIter);

    // Pausa para visualização final
    cout << "\nPressione Enter para sair...";
    cin.ignore();
    cin.get();

    return 0;
}
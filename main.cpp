/*
 * ATIVIDADE PRÁTICA 01 - MÉTODO DE NEWTON-RAPHSON
 * Disciplina: Métodos Numéricos e Computacionais
 * Descrição: Implementação com Derivada Automática (Diferenças Finitas).
 */

// ==========================================================
// INCLUSÃO DE BIBLIOTECAS
// ==========================================================
#include <iostream>     // Para entrada e saída padrão (cin, cout)
#include <cmath>        // Para funções matemáticas (pow, sin, cos, exp, abs, etc.)
#include <iomanip>      // Para manipulação de formatação (setw, setprecision)
#include <functional>   // Para usar std::function (permite passar funções como parâmetros)
#include <string>       // Para manipulação de strings
#include <cctype>       // Para funções de caracteres (isdigit, isalpha, isspace)
#include <stdexcept>    // Para tratamento de exceções (runtime_error)
#include <fstream>      // Para manipulação de arquivos (salvar o CSV)

using namespace std;

// ==========================================================
// CLASSE: SIMPLE PARSER
// Responsável por interpretar strings matemáticas (ex: "x^2 + sin(x)")
// e calcular o resultado numérico.
// ==========================================================
class SimpleParser {
private:
    string expr;        // A expressão matemática em texto
    int pos;            // Posição atual do cursor de leitura na string
    long double x_val;  // O valor numérico atual da variável 'x'

    // Verifica o caractere atual sem avançar o cursor (ignora espaços)
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

    // Lida com fatores: números, parênteses, sinal negativo unário e funções matemáticas
    long double parseFactor() {
        char c = peek();
        // Se for número
        if (isdigit(c) || c == '.') return parseNumber();
        
        // Se for parênteses, resolve a expressão interna recursivamente
        if (c == '(') {
            get(); 
            long double result = parseExpression();
            if (get() != ')') throw runtime_error("Esperado ')'");
            return result;
        }
        
        // Se for sinal negativo (ex: -5)
        if (c == '-') {
            get();
            return -parseFactor();
        }
        
        // Se for letra, verifica se é variável 'x' ou funções (sin, cos, etc.)
        if (isalpha(c)) {
            string name;
            while (isalpha(peek())) name += get();
            
            if (name == "x") return x_val;
            else if (name == "sin") return sin(parseFactor());
            else if (name == "cos") return cos(parseFactor());
            else if (name == "tan") return tan(parseFactor());
            else if (name == "exp") return exp(parseFactor());
            else if (name == "log") return log(parseFactor()); // Log natural (ln)
            else if (name == "sqrt") return sqrt(parseFactor());
            else throw runtime_error("Funcao desconhecida: " + name);
        }
        throw runtime_error("Caractere inesperado: " + string(1, c));
    }

    // Lida com exponenciação (^), que tem prioridade sobre multiplicação
    long double parsePower() {
        long double left = parseFactor();
        while (peek() == '^') {
            get();
            long double right = parseFactor();
            left = pow(left, right);
        }
        return left;
    }

    // Lida com Multiplicação (*) e Divisão (/)
    long double parseTerm() {
        long double left = parsePower(); // Resolve potências primeiro
        while (peek() == '*' || peek() == '/') {
            char op = get();
            long double right = parsePower();
            if (op == '*') left *= right;
            else {
                if (abs(right) < 1e-15L) throw runtime_error("Divisao por zero detectada na expressao");
                left /= right;
            }
        }
        return left;
    }

    // Ponto de entrada da recursão: Soma (+) e Subtração (-)
    // Esta função chama parseTerm, que chama parsePower, que chama parseFactor.
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
    // Adiciona multiplicação implícita (ex: "3x" vira "3*x") para facilitar o parsing
    string preprocess(string s) {
        string res = "";
        for (size_t i = 0; i < s.length(); i++) {
            res += s[i];
            if (i + 1 < s.length()) {
                // Se temos um dígito seguido de uma letra (ex: 2x), insere *
                if (isdigit(s[i]) && isalpha(s[i+1])) {
                    res += '*';
                }
            }
        }
        return res;
    }

    // Função pública para calcular o valor da expressão dado um x
    long double evaluate(string expression_str, long double x) {
        this->expr = preprocess(expression_str);
        this->pos = 0;
        this->x_val = x;
        return parseExpression();
    }
    
    // Retorna a string corrigida (com multiplicações explícitas) para exibição
    string getFixedString(string s) {
        return preprocess(s);
    }
};

// ==========================================================
// FUNÇÕES PREDEFINIDAS
// Exemplos prontos para teste rápido (Hardcoded)
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
// Recebe:
// f: A função original
// df: A derivada da função
// x0: Chute inicial
// epsilon: Tolerância de erro
// maxIter: Limite de iterações para evitar loops infinitos
// ==========================================================
void newtonRaphson(
    std::function<long double(long double)> f,
    std::function<long double(long double)> df,
    long double x0,
    long double epsilon,
    int maxIter
) {
    long double x_atual = x0;
    long double x_prox;
    long double erro = epsilon + 1.0L; // Inicializa erro maior que epsilon para entrar no loop
    int iter = 0;

    // Configuração do arquivo de saída CSV
    ofstream arquivoCsv;
    arquivoCsv.open("tabela.csv"); 
    arquivoCsv << "k,x_n,f(x_n),Erro Estimado" << endl; // Cabeçalho do CSV
    arquivoCsv << fixed << setprecision(15); 

    // Configuração da saída no console (Tabela formatada)
    cout << "\n" << string(130, '-') << endl;
    cout << "TABELA DE ITERACOES (Metodo de Newton-Raphson)" << endl;
    cout << string(130, '-') << endl;
    cout << left << setw(10) << "k" 
         << left << setw(40) << "x_n" 
         << left << setw(40) << "f(x_n)" 
         << left << setw(40) << "Erro Estimado" << endl;
    cout << string(130, '-') << endl;
    cout << fixed << setprecision(12);

    // Loop principal do método
    while (iter < maxIter) {
        long double fx, dfx;
        
        try {
            // Calcula f(x) e f'(x)
            fx = f(x_atual);
            dfx = df(x_atual); 
        } catch (const exception& e) {
            cout << "\n[ERRO MATEMATICO] " << e.what() << endl;
            arquivoCsv << "ERRO," << e.what() << endl;
            arquivoCsv.close();
            return;
        }

        // Proteção contra divisão por zero na fórmula de Newton
        if (abs(dfx) < 1e-18L) { 
            cout << "\n[ERRO] Derivada anulou-se em x = " << x_atual << endl;
            arquivoCsv.close();
            return;
        }

        // FÓRMULA DE NEWTON-RAPHSON: x(n+1) = x(n) - f(x)/f'(x)
        x_prox = x_atual - (fx / dfx);
        
        // Cálculo do erro absoluto entre iterações
        erro = abs(x_prox - x_atual);

        // Exibição dos dados da iteração atual
        cout << left << setw(10) << iter + 1
             << left << setw(40) << x_atual
             << left << setw(40) << fx
             << left << setw(0) << erro << endl;

        // Gravação no arquivo
        arquivoCsv << (iter + 1) << "," 
                   << x_atual << "," 
                   << fx << "," 
                   << erro << endl;

        // Atualiza x para a próxima iteração
        x_atual = x_prox;
        iter++;

        // Critério de Parada: Convergência atingida
        if (erro < epsilon) {
            cout << string(130, '-') << endl;
            cout << "\nRESULTADO FINAL:" << endl;
            cout << "Status: CONVERGIU COM SUCESSO" << endl;
            cout << "Raiz Aproximada: " << x_atual << endl;
            cout << "Total de Iteracoes: " << iter << endl;
            cout << "\n[INFO] Arquivo 'tabela.csv' gerado." << endl;
            arquivoCsv.close(); 
            return;
        }
    }

    // Critério de Parada: Limite de iterações excedido (Divergência ou Oscilação)
    cout << string(130, '-') << endl;
    cout << "\nRESULTADO FINAL:" << endl;
    cout << "Status: NAO CONVERGIU (Limite de iteracoes atingido)" << endl;
    cout << "Ultima aproximacao: " << x_atual << endl;
    
    arquivoCsv.close();
}

// ==========================================================
// FUNÇÃO PRINCIPAL
// ==========================================================
int main() {
    int escolha;
    long double x0, epsilon;
    int maxIter;
    SimpleParser parser; // Instancia o interpretador de expressões

    string userFuncStr, userDerivStr;
    // std::function armazena qualquer "chamável" (função normal ou lambda)
    std::function<long double(long double)> f_selecionada;
    std::function<long double(long double)> df_selecionada;
    string desc_f;

    // Menu de opções
    cout << "=== IMPLEMENTACAO: METODO DE NEWTON-RAPHSON ===" << endl;
    cout << "1. f(x) = x^3 - 2x - 5" << endl;
    cout << "2. f(x) = cos(x) - x" << endl;
    cout << "3. f(x) = e^(-x) - x" << endl;
    cout << "4. f(x) = x^3 - 9x + 3" << endl;
    cout << "5. DIGITAR MINHA PROPRIA FUNCAO" << endl;
    cout << "Opcao: ";
    cin >> escolha;

    if (escolha == 5) {
        cout << "\n--- CONFIGURACAO DE FUNCAO ---" << endl;
        cout << "Digite a funcao f(x): ";
        cin.ignore(); // Limpa o buffer do teclado
        getline(cin, userFuncStr);
        
        // Define f_selecionada usando uma função Lambda que chama o Parser
        f_selecionada = [&parser, userFuncStr](long double x) {
            return parser.evaluate(userFuncStr, x);
        };

        // Opção de Derivada Automática
        char calcDerivAuto;
        cout << "Deseja que o programa calcule a derivada automaticamente? (S/N): ";
        cin >> calcDerivAuto;

        if (toupper(calcDerivAuto) == 'S') {
            cout << "[Sistema] Modo Derivada Numerica ativado." << endl;
            
            // Lógica de Diferenciação Numérica (Diferença Finita Central)
            // Fórmula: f'(x) ~ (f(x+h) - f(x-h)) / 2h
            // Isso permite calcular a derivada sem saber a fórmula analítica dela.
            df_selecionada = [&parser, userFuncStr](long double x) {
                long double h = 1e-8L; // Um passo 'h' muito pequeno
                long double fx_plus = parser.evaluate(userFuncStr, x + h);
                long double fx_minus = parser.evaluate(userFuncStr, x - h);
                return (fx_plus - fx_minus) / (2.0L * h);
            };
            
        } else {
            // Usuário digita a derivada manualmente
            cout << "Digite a derivada f'(x): ";
            cin.ignore(); 
            getline(cin, userDerivStr);
            
            string fixedD = parser.getFixedString(userDerivStr);
            cout << "[Sistema] Derivada Manual: " << fixedD << endl;
            
            df_selecionada = [&parser, userDerivStr](long double x) {
                return parser.evaluate(userDerivStr, x);
            };
        }

        string fixedF = parser.getFixedString(userFuncStr);
        cout << "[Sistema] Funcao f(x): " << fixedF << endl;

    } else {
        // Seleção das funções hardcoded (Predefinidas)
        switch (escolha) {
            case 1: f_selecionada = func1; df_selecionada = deriv1; break;
            case 2: f_selecionada = func2; df_selecionada = deriv2; break;
            case 3: f_selecionada = func3; df_selecionada = deriv3; break;
            case 4: f_selecionada = func4; df_selecionada = deriv4; break;
            default: cout << "Opcao invalida!" << endl; return 1;
        }
    }

    // Coleta de parâmetros numéricos
    cout << "\n--- Parametros de Execucao ---" << endl;
    cout << "Digite o chute inicial (x0): ";
    cin >> x0;
    cout << "Digite o erro maximo permitido (epsilon): ";
    cin >> epsilon;
    cout << "Digite o numero maximo de iteracoes: ";
    cin >> maxIter;

    // Executa o algoritmo
    newtonRaphson(f_selecionada, df_selecionada, x0, epsilon, maxIter);

    cout << "\nPressione Enter para sair...";
    cin.ignore();
    cin.get();

    return 0;
}
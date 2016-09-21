RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color
#echo -e "I ${RED}love${NC} Stack Overflow\n"

#
# ordem de acoes de compilacao:
# aplicar pacth do 14.04 no player
# aplicar patch de proxies
# instalar todas as dependencias do player
# compilar player
# criar cariavel de ambiente DONNIE_PATH, esta variavle vai ter dir
  # lib/player, onde vao ficar todos os .so
  # lib/ros, futuro
  # resources, onde vao ficar todos arquivos de som
  # simul/stage, recursos do stage
  # simul/gazebo, para futuro
  # test/player, clientes de teste dos drivers player, cfg de teste, etc
# para cada driver, instalar suas dependencias, o driver, e cliente de teste qnd houver
# compilar o interpretador
# compilar documentacao latex


# TODO: dar um grep recursivo por -lboost_system e colocar esta lib no pkg-cfg do player. procurar arquivo .pc
# TODO: nem todos os cmakes dos drivers possuem uma regra make install. Tudo q eh importante deve estar no install, inclusives cfgs e clientes de teste
# TODO:  fazer grep recursivo por TODO para consertar as pendencias
# TODO:  nenhum CFG pode ter caminho absoluto. todos devem apontar para o diretorio DONNIE_PATH/lib/player/





echo -e "Compiling ${GREEN}Donnie Driver${NC} ..."
cd driver/donnie
# pegar o codigo de retorno p saber se foi executado corretamente
./compile.sh
cd ../../

#to compile soxplayer driver
sudo apt-get install -y sox
sudo apt-get install -y libsox-dev 
sudo apt-get install -y libsox-fmt-all
#to compile gtts driver
sudo apt-get install -y curl
sudo apt-get install -y libcurl4-openssl-dev
#to compile Logo Client
sudo apt-get install -y libreadline-dev
sudo apt-get install -y libantlr3c-dev


cd driver/soxplayer
./compile.sh
cd client-test
./compile.sh
cd ../../../

cd driver/gtts
./compile.sh
cd client-test
./compile.sh
cd ../../../

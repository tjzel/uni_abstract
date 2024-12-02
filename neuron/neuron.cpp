#include <iostream>
#include <vector>

template <typename TSelf> struct Neurons {
private:
  Neurons() = default;

public:
  template <typename TOther> void connect(TOther &other) {
    for (auto &from : *static_cast<TSelf *>(this)) {
      for (auto &to : other) {
        from.out.push_back(&to);
        to.in.push_back(&from);
      }
    }
  }
  TSelf *begin() { return static_cast<TSelf *>(this)->begin(); }
  TSelf *end() { return static_cast<TSelf *>(this)->end(); }
  friend TSelf;
};

struct Neuron : Neurons<Neuron> {
  std::vector<Neuron *> in;
  std::vector<Neuron *> out;
  unsigned int ID;
  Neuron() {
    static int ID = 1;
    this->ID = ID++;
  }
  // like a vector...
  Neuron *begin() { return this; }
  Neuron *end() { return this + 1; }
};

struct NeuronLayer : std::vector<Neuron>, Neurons<NeuronLayer> {
  using std::vector<Neuron>::begin;
  using std::vector<Neuron>::end;
  explicit NeuronLayer(int number_of_neurons) : std::vector<Neuron>(number_of_neurons) {}
};

template <typename TSelf> std::ostream &operator<<(std::ostream &console, Neurons<TSelf> &neurons) {
  for (auto neuronIter = static_cast<TSelf *>(&neurons)->begin(); neuronIter != static_cast<TSelf *>(&neurons)->end();
       ++neuronIter) {
    for (auto *outgoingIter : neuronIter->out) {
      console << neuronIter->ID << "*" << " -> " << outgoingIter->ID << "\n";
    }
    for (auto *incomingIter : neuronIter->in) {
      console << incomingIter->ID << " -> " << neuronIter->ID << "*" << "\n";
    }
  }
  return console;
}

int main() {
  Neuron single_neuron_1, single_neuron_2;
  NeuronLayer layer_1{1}, layer_2{2};
  single_neuron_1.connect(single_neuron_2);
  single_neuron_1.connect(layer_1);
  layer_1.connect(layer_2);
  layer_2.connect(single_neuron_2);
  std::cout << single_neuron_1 << "\n";
  std::cout << single_neuron_2 << "\n";
  std::cout << layer_1 << "\n";
  std::cout << layer_2 << "\n";
  return 0;
}

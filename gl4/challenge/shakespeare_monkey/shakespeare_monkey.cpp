/* genetic algorithm, mimic the theory of evolution:
 *  heredity. There must be a process in place by which children receive the
 *  properties of parents
 *
 *  variation. There must be a variety of traits present in the population or a
 *  means with which to introduce variation.
 *
 *  selection. There must be a mechanism by which some members of a population
 *  have the opportunity to be parents and pass down their genetic information
 *  and some do not. This is typically refered to as "survival of the fittest"
 *
 *
 * algorithm:
 *  step 1:
 *    initialize: create a population of N elements, each with randomly
 *    generated DNA.
 *
 *  step 2: Selectoin: Evaluate the fitness of each element of the population and
 *  build a mating pool.
 *
 *  step 3: Reproduction: Repeat N times:
 *    1. Pick two parents with probability according to relative fitness.
 *    2. Crossover - create a "child" by combining the DNA of these two parents.
 *    3. Mutation - mutate the child's DNA based on a given probability
 *    d. Add the new child to a new populatoin
 *
 *  step 4. Replace the old elements with the new elements and return to step 2
 *
 * some concept:
 *  genotype: dna data
 *  phenotype: actual expression
 *
 *-----------------------------------------------------------------------------------
 * shakespeare monkey problem:
 *  give a monkey a typewriter, give it enough time, the monkey will type every
 *  time in all of the shakespeare book.
 */
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <numeric>
#include <ctime>
#include <algorithm>

// return [0:1)
float unit_random()
{
  return static_cast<float>( (rand() % RAND_MAX)) / RAND_MAX;
}

int random_by_weight(const std::vector<float>& weights)
{
  // pick parents
  float w = unit_random() * weights.back();
  if(w == weights.back()) // happens even unit_random() doesn't return 1
    return weights.size() - 1;

  auto iter = std::upper_bound(weights.begin(), weights.end(), w);
  if(iter == weights.end())
  {
    std::cout << "w : " << w << std::endl;
    for(const auto& item : weights)
    {
      std::cout << item << " ";
    }
    std::cout << std::endl;
    throw std::runtime_error("weight vector error");
  }
  return std::distance(weights.begin(), iter);
}


class dna
{
protected:
  std::string m_genes;
  float m_fitness = 0;
public:

  dna(int length)
  {
    m_genes.reserve(length);
    while(length--)
    {
      m_genes.push_back(rand_ascii());
    }
  }

  void calc_fitness(const std::string& genes)
  {
    m_fitness = 0;
    for (int i = 0; i < m_genes.size(); ++i)
    {
      if(m_genes[i] == genes[i])
        ++m_fitness;
    }
    m_fitness /= m_genes.size();
    m_fitness *= m_fitness;
    m_fitness *= m_fitness;
  }

  // create new child
  dna cross_over(dna& rhs)
  {
    dna child(*this);
    int i = m_genes.size() / 2;
    std::copy(rhs.m_genes.begin() + i, rhs.m_genes.end(), m_genes.begin() + i);
    return child;
  }

  void mutate(const std::string& desire, float rate)
  {
      // should i mutate the tood gene too?
    for (int i = 0; i < m_genes.size(); ++i)
    {
      if(m_genes[i] == desire[i])
        continue;
      if(unit_random() < rate)
        m_genes[i] = rand_ascii();
    }
  }

  const std::string& genes() const { return m_genes; }
  void genes(const std::string& v){ m_genes = v; }

  float fitness() const { return m_fitness; }
  void fitness(float v){ m_fitness = v; }

protected:
  char rand_ascii()
  {
    // only ox20 - ox7f is printable character
    return 0x20 + rand() % 96;
  }

};

typedef std::vector<dna> dna_vector;

class population
{
protected:
  int m_size; // size of pupulation
  float m_mutation_rate;
  std::string m_desire_genes;
  const dna* m_best; // current best

  dna_vector m_dnas;
  
public:
  population(const std::string& s, int size, float mutation):
    m_desire_genes(s),
    m_size(size),
    m_mutation_rate(mutation)
  {
    // initialize
    for (int i = 0; i < m_size; ++i)
    {
      m_dnas.emplace_back(m_desire_genes.size());
    }
  }

  void evolve()
  {
    evaluate();
    int i = 0;
    while(m_best->fitness() != 1)
    {
      std::cout << "generation " << ++i << " : fitness "  << m_best->fitness() << " : " << m_best->genes() << std::endl;
      reproduce();
      evaluate();
    }
    std::cout << "generation " << ++i << " : fitness "  << m_best->fitness() << " : " << m_best->genes() << std::endl;
  }

protected:
  void evaluate()
  {
    m_best = &m_dnas.front();
    for(auto& item : m_dnas)
    {
      item.calc_fitness(m_desire_genes);
      if(item.fitness() > m_best->fitness())
        m_best = &item;
    }
  }

  // rejection sampling, use fitness as weight to pick dna
  dna* pick_by_fitness()
  {
    float best_fitness = m_best->fitness();
    while(true)
    {
      dna* d = &m_dnas[rand() % m_size];
      // use another random to apply fitness as selection weight
      if(best_fitness == 0 || unit_random() * best_fitness <  d->fitness())
        return d;
    }
  }

  void reproduce()
  {
    // build weights

    dna_vector dnas;
    dnas.reserve(m_size);
    for (int i = 0; i < m_size; ++i)
    {
      dna* parent0 = pick_by_fitness();
      dna* parent1 = pick_by_fitness();
      dnas.push_back(parent0->cross_over(*parent1));
      dnas.back().mutate(m_desire_genes, m_mutation_rate);
    }

    m_dnas = dnas;

    // build weights
    //std::vector<float> weights;
    //weights.reserve(m_dnas.size());
    //for(const auto& item : m_dnas)
    //{
      //float w = item.fitness();
      //if(!weights.empty())
        //w += weights.back();
      //weights.push_back(w);
    //}

    //dna_vector dnas;
    //dnas.reserve(m_size);

    //for (int i = 0; i < m_size; ++i)
    //{
      //dna* parent0 = &m_dnas[random_by_weight(weights)];
      //dna* parent1 = &m_dnas[random_by_weight(weights)];
      //dnas.push_back(parent0->cross_over(*parent1));
      //dnas.back().mutate(m_desire_genes, m_mutation_rate);
    //}

    //m_dnas = dnas;

    //another way to pick parent is to build a mate pool according to fitness
    //std::vector<dna*> pool;
    //pool.reserve(m_size);
    //for (int i = 0; i < m_size; ++i)
    //{
      //int fitness = m_dnas[i].fitness() * m_size;
      //for (int j = 0; j < fitness; ++j)
      //{
        //pool.push_back(&m_dnas[i]);
      //}
    //}

    //dna_vector dnas;
    //dnas.reserve(m_size);

    //for (int i = 0; i < m_size; ++i)
    //{
      //dna* parent0 = pool[rand()%pool.size()];
      //dna* parent1 = pool[rand()%pool.size()];
      //dnas.push_back(parent0->cross_over(*parent1));
      //dnas.back().mutate(m_desire_genes, m_mutation_rate);
    //}


  }
};


int main(int argc, char *argv[]) 
{
  srand(time(0));
  population p("Oh my god! when does winter come! It has been seven years!!! Oh my god!", 1000, 0.01);
  p.evolve();
  return 0;
}

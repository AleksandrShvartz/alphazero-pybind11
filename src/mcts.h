#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "game_state.h"
#include "shapes.h"

namespace alphazero {

struct Node {
  Node() = default;
  explicit Node(uint32_t m) : move(m){};

  float q = 0;
  float policy = 0;
  uint32_t move = 0;
  uint32_t n = 0;
  int8_t player = 0;
  std::optional<Vector<float>> scores = std::nullopt;
  std::vector<Node> children{};

  void add_children(const Vector<uint8_t>& valids) noexcept;
  void update_policy(const Vector<float>& pi) noexcept;
  [[nodiscard]] float uct(float sqrt_parent_n, float cpuct) const noexcept;
  [[nodiscard]] Node* best_child(float cpuct) noexcept;
};

class MCTS {
 public:
  MCTS(float cpuct, uint32_t num_moves, float epsilon = 0)
      : cpuct_(cpuct),
        num_moves_(num_moves),
        current_(&root_),
        epsilon_(epsilon) {}
  void update_root(const GameState& gs, uint32_t move);
  [[nodiscard]] std::unique_ptr<GameState> find_leaf(const GameState& gs);
  void process_result(const GameState& gs, Vector<float>& value,
                      Vector<float>& pi, bool root_noise_enabled = false);
  void add_root_noise();
  [[nodiscard]] float root_value() const {
    float q = 0;
    for (const auto& c : root_.children) {
      if (c.q > q) {
        q = c.q;
      }
    }
    return q;
  }
  [[nodiscard]] Vector<uint32_t> counts() const noexcept;
  [[nodiscard]] Vector<float> probs(float temp) const noexcept;
  [[nodiscard]] uint32_t depth() const noexcept { return depth_; };

  [[nodiscard]] static uint32_t pick_move(const Vector<float>& p);

 private:
  float cpuct_;
  int32_t num_moves_;

  uint32_t depth_ = 0;
  Node root_ = Node{};
  Node* current_;
  std::vector<Node*> path_{};
  float epsilon_;
};

}  // namespace alphazero
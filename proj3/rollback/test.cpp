struct struct_t {
  void change(int t) {
    member = t;
  }
  int member {0};
};

int main() {
  struct_t t {};
  const struct_t* r = &t;
  r->change(3);

  return 0;
}

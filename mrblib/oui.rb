module OUI
  class Context
    def create_layout
      begin_layout
      yield self
      end_layout
    end
  end
end
